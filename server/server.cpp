#include <model.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <utility>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

using json = nlohmann::json;

class Database {
private:
    pqxx::connection C;

public:
    explicit Database(pqxx::connection &&C_) : C(std::move(C_)) {
        pqxx::work W(C);
        std::string create_table;
        try {
            create_table =
                    "CREATE TABLE IF NOT EXISTS Users (user_id BIGSERIAL NOT NULL "
                    "PRIMARY KEY, user_name VARCHAR(50) NOT NULL UNIQUE)";
            W.exec(create_table);
        } catch (std::exception const &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
        try {
            create_table =
                    "CREATE TABLE IF NOT EXISTS Chats (chat_id BIGSERIAL NOT NULL "
                    "PRIMARY KEY, chat_name VARCHAR(50) NOT NULL UNIQUE)";
            W.exec(create_table);
        } catch (std::exception const &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
        try {
            create_table =
                    "CREATE TABLE IF NOT EXISTS UserID_ChatID (user_id BIGINT NOT "
                    "NULL, chat_id BIGINT NOT NULL, FOREIGN KEY(user_id) "
                    "REFERENCES Users(user_id), FOREIGN KEY(chat_id) "
                    "REFERENCES Chats(chat_id))";
            W.exec(create_table);
        } catch (std::exception const &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
        try {
            create_table =
                    "CREATE TABLE IF NOT EXISTS Messages (chat_id BIGINT NOT NULL, "
                    "sender_id BIGINT NOT NULL, recipient_id BIGINT NOT NULL, "
                    "message_text TEXT, FOREIGN KEY(chat_id) REFERENCES "
                    "Chats(chat_id), FOREIGN KEY(sender_id) REFERENCES "
                    "Users(user_id), FOREIGN KEY(recipient_id) REFERENCES "
                    "Users(user_id))";
            W.exec(create_table);
        } catch (std::exception const &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
        W.commit();
    }

    void add_message(std::size_t chat_id,
                     std::size_t sender_id,
                     std::size_t recipient_id,
                     std::string &message_text) {
        pqxx::work W(C);
        try {
            std::string insert_into =
                    "INSERT INTO Messages (chat_id, sender_id, recipient_id, "
                    "message_text) Values (" +
                    std::to_string(chat_id) + ", " + std::to_string(sender_id) +
                    ", " + std::to_string(recipient_id) + ", '" + message_text +
                    "')";
            W.exec(insert_into);
            W.commit();
        } catch (std::exception const &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
    }

    void add_chat(std::string &chat_name) {
        pqxx::work W(C);
        try {
            std::string insert_into =
                    "INSERT INTO Chats (chat_name) Values ('" + chat_name + "')";
            W.exec(insert_into);
            W.commit();
        } catch (std::exception const &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
    }

    void add_user(std::string &user_name) {
        pqxx::work W(C);
        try {
            std::string insert_into =
                    "INSERT INTO Users (user_name) Values ('" + user_name + "')";
            W.exec(insert_into);
            W.commit();
        } catch (std::exception const &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
    }

    void link_user_to_chat(std::size_t user_id, std::size_t chat_id) {
        pqxx::work W(C);
        try {
            std::string insert_into =
                    "INSERT INTO UserID_ChatID (user_id, chat_id) Values (" +
                    std::to_string(user_id) + ", " + std::to_string(chat_id) + ")";
            W.exec(insert_into);
            W.commit();
        } catch (std::exception const &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
    }

    std::string find_user(std::size_t user_id) {
        pqxx::work W(C);
        try {
            std::string query = "SELECT user_id, user_name FROM Users";
            for (auto [id, name] : W.stream<std::size_t, std::string>(query)) {
                if (id == user_id) {
                    return json{{"user_id", id}, {"user_name", name}}.dump(4);
                }
            }
            return "User not found";
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
    }

    std::string get_chat_history(std::size_t chat_id) {
        pqxx::work W(C);
        try {
            json j;
            std::string query =
                    "SELECT chat_id, sender_id, recipient_id, message_text FROM "
                    "Messages";
            for (auto [id, sender_id, recipient_id, message_text] :
                    W.stream<std::size_t, std::size_t, std::size_t, std::string>(
                            query)) {
                if (id == chat_id) {
                    j.push_back(json{{"chat_id", chat_id},
                                     {"sender_id", sender_id},
                                     {"recipient_id", recipient_id},
                                     {"message_text", message_text}});
                }
            }
            return j.dump(4);
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
    }

    std::string get_chat_list(std::size_t user_id) {
        pqxx::work W(C);
        try {
            json j;
            std::string query = "SELECT user_id, chat_id FROM UserID_ChatID";
            for (auto [id, chat_id] :
                    W.stream<std::size_t, std::size_t>(query)) {
                if (id == user_id) {
                    j.push_back(json{{"chat_id", chat_id}});
                }
            }
            return j.dump(4);
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
    }
};

typedef websocketpp::server<websocketpp::config::asio> server;
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
typedef server::message_ptr message_ptr;

// maybe not needed
void on_http(server *s, const websocketpp::connection_hdl &hdl) {
    server::connection_ptr con = s->get_con_from_hdl(hdl);

    std::string res = con->get_request_body();

    std::stringstream ss;
    ss << "got HTTP request with " << res.size() << " bytes of body data.";

    con->set_body(ss.str());
    con->set_status(websocketpp::http::status_code::ok);
}

void on_fail(server *s, const websocketpp::connection_hdl &hdl) {
    websocketpp::server<websocketpp::config::asio>::connection_ptr con =
            s->get_con_from_hdl(hdl);
    websocketpp::lib::error_code ec = con->get_ec();
    if (ec) {
        std::cerr << "Connection attempt by client failed because: "
                  << ec.message() << std::endl;
    }
}
void on_close(server *s, const websocketpp::connection_hdl &hdl) {
    websocketpp::lib::error_code ec;
    s->close(hdl, 400, "Websocket closed", ec);
}

void on_message(server *s,
                Database *database,
                const websocketpp::connection_hdl &hdl,
                const message_ptr &msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload() << std::endl;

    if (msg->get_payload() == "stop-listening") {
        s->stop_listening();
        return;
    }
    std::string ECHO_CMD = "echo";
    std::string FIND_USER_CMD = "find_user";
    std::string GET_CHAT_HISTORY = "get_chat_history";
    std::string GET_CHAT_LIST = "get_chat_list";

    try {
        if (msg->get_payload().substr(0, ECHO_CMD.size()) == "echo") {
            s->send(hdl, msg->get_payload().substr(ECHO_CMD.size() + 1),
                    msg->get_opcode());
        } else if (msg->get_payload().substr(0, FIND_USER_CMD.size()) ==
                   "find_user") {
            std::size_t user_id =
                    std::stoi(msg->get_payload().substr(FIND_USER_CMD.size() + 1));
            std::string message = database->find_user(user_id);
            s->send(hdl, message, msg->get_opcode());
        } else if (msg->get_payload().substr(0, GET_CHAT_HISTORY.size()) ==
                   "get_chat_history") {
            std::size_t chat_id = std::stoi(
                    msg->get_payload().substr(GET_CHAT_HISTORY.size() + 1));
            std::string message = database->get_chat_history(chat_id);
            s->send(hdl, message, msg->get_opcode());
        } else if (msg->get_payload().substr(0, GET_CHAT_LIST.size()) ==
                   "get_chat_list") {
            std::size_t user_id =
                    std::stoi(msg->get_payload().substr(GET_CHAT_LIST.size() + 1));
            std::string message = database->get_chat_list(user_id);
            s->send(hdl, message, msg->get_opcode());
        } else {
            s->send(hdl, "Unrecognized command", msg->get_opcode());
        }
    } catch (websocketpp::exception const &e) {
        std::cerr << "Send failed because: "
                  << "(" << e.what() << ")" << std::endl;
        throw;
    }
}

int main() {
    Database database(pqxx::connection(
            "user=buyantuev-alexander"));  //пока подключение к localhost

    try {
        server server;
        server.set_access_channels(websocketpp::log::alevel::all);
        server.clear_access_channels(websocketpp::log::alevel::frame_payload);
        server.init_asio();
        server.set_http_handler(bind(&on_http, &server, ::_1));
        server.set_fail_handler(bind(&on_fail, &server, ::_1));
        server.set_close_handler(bind(&on_close, &server, ::_1));
        server.set_message_handler(
                bind(&on_message, &server, &database, ::_1, ::_2));
        server.listen(9002);
        std::cout << "Listening on 9002" << std::endl;
        server.start_accept();
        server.run();
    } catch (websocketpp::exception const &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Caught other exception" << std::endl;
        return 1;
    }
    return 0;
}