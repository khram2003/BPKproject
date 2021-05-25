#include <model.h>
#include <fstream>
#include <future>
#include <iostream>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <utility>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

using json = nlohmann::json;

namespace {
std::ofstream server_log("server_log.txt", std::ifstream::app);
const std::string request = "REQUEST: ";
const std::string error = "ERROR: ";
}  // namespace

class Database {
private:
    pqxx::connection C;

    const std::string success = "SUCCESS";
    const std::string fail = "FAIL";

public:
    Database() = default;

    explicit Database(pqxx::connection &&C_) : C(std::move(C_)) {
        pqxx::work W(C);
        std::string create_table;
        std::string error_message =
            "Failed to create table, check your PostgreSQL database";
        try {
            create_table =
                "CREATE TABLE IF NOT EXISTS Users (user_id BIGSERIAL NOT NULL "
                "PRIMARY KEY, user_name VARCHAR(50) NOT NULL UNIQUE)";
            W.exec(create_table);
        } catch (std::exception const &e) {
            std::cerr << error << error_message << std::endl;
        }
        try {
            create_table =
                "CREATE TABLE IF NOT EXISTS Chats (chat_id BIGSERIAL NOT NULL "
                "PRIMARY KEY, chat_name VARCHAR(50) NOT NULL UNIQUE)";
            W.exec(create_table);
        } catch (std::exception const &e) {
            std::cerr << error << error_message << std::endl;
        }
        try {
            create_table =
                "CREATE TABLE IF NOT EXISTS UserID_ChatID (user_id BIGINT NOT "
                "NULL, chat_id BIGINT NOT NULL, FOREIGN KEY(user_id) "
                "REFERENCES Users(user_id), FOREIGN KEY(chat_id) "
                "REFERENCES Chats(chat_id))";
            W.exec(create_table);
        } catch (std::exception const &e) {
            std::cerr << error << error_message << std::endl;
        }
        try {
            create_table =
                "CREATE TABLE IF NOT EXISTS Messages (chat_id BIGINT NOT NULL, "
                "sender_id BIGINT NOT NULL, message_text TEXT, FOREIGN "
                "KEY(chat_id) REFERENCES "
                "Chats(chat_id), FOREIGN KEY(sender_id) REFERENCES "
                "Users(user_id))";
            W.exec(create_table);
        } catch (std::exception const &e) {
            std::cerr << error << error_message << std::endl;
        }
        try {
            create_table =
                "CREATE TABLE IF NOT EXISTS UserID_TrelloToken (user_id BIGINT "
                "NOT NULL, trello_token TEXT NOT NULL UNIQUE, FOREIGN "
                "KEY(user_id) REFERENCES Users(user_id))";
            W.exec(create_table);
        } catch (std::exception const &e) {
            std::cerr << error << error_message << std::endl;
        }
        W.commit();
    }

    std::string add_message(std::size_t chat_id,
                            std::size_t sender_id,
                            std::string &message_text) {
        pqxx::work W(C);
        try {
            std::string insert_into =
                "INSERT INTO Messages (chat_id, sender_id, message_text) "
                "Values (" +
                std::to_string(chat_id) + ", " + std::to_string(sender_id) +
                ", '" + message_text + "')";
            W.exec(insert_into);
            W.commit();
            return success;
        } catch (std::exception const &e) {
            server_log << error << e.what() << std::endl;
            return fail;
        }
    }

    std::string add_chat(std::string &chat_name) {
        pqxx::work W(C);
        try {
            std::string insert_into =
                "INSERT INTO Chats (chat_name) Values ('" + chat_name + "')";
            W.exec(insert_into);
            W.commit();
            return success;
        } catch (std::exception const &e) {
            server_log << error << e.what() << std::endl;
            return fail;
        }
    }

    std::string add_user(std::string &user_name) {
        pqxx::work W(C);
        try {
            std::string insert_into =
                "INSERT INTO Users (user_name) Values ('" + user_name + "')";
            W.exec(insert_into);
            W.commit();
            return success;
        } catch (std::exception const &e) {
            server_log << error << e.what() << std::endl;
            return fail;
        }
    }

    std::string link_user_to_chat(std::size_t user_id, std::size_t chat_id) {
        pqxx::work W(C);
        try {
            std::string insert_into =
                "INSERT INTO UserID_ChatID (user_id, chat_id) Values (" +
                std::to_string(user_id) + ", " + std::to_string(chat_id) + ")";
            W.exec(insert_into);
            W.commit();
            return success;
        } catch (std::exception const &e) {
            server_log << error << e.what() << std::endl;
            return fail;
        }
    }

    std::string find_user(const std::string &user_name) {
        pqxx::work W(C);
        try {
            std::string query =
                "SELECT user_id, user_name FROM Users WHERE user_name = '" +
                user_name + "'";
            for (auto [id, name] : W.stream<std::size_t, std::string>(query)) {
                return json{{"user_id", id}, {"user_name", name}}.dump();
            }
            return "User not found";
        } catch (const std::exception &e) {
            server_log << error << e.what() << std::endl;
            return fail;
        }
    }

    std::string get_chat_history(std::size_t chat_id) {
        pqxx::work W(C);
        try {
            json j;
            std::string query =
                "SELECT chat_id, sender_id, message_text FROM "
                "Messages WHERE chat_id = " +
                std::to_string(chat_id);
            for (auto [id, sender_id, message_text] :
                 W.stream<std::size_t, std::size_t, std::string>(query)) {
                j.push_back(json{{"chat_id", chat_id},
                                 {"sender_id", sender_id},
                                 {"message_text", message_text}});
            }
            return j.dump();
        } catch (const std::exception &e) {
            server_log << error << e.what() << std::endl;
            return fail;
        }
    }

    std::string get_chat_name(std::size_t chat_id) {
        pqxx::work W(C);
        try {
            json j;
            std::string query =
                "SELECT chat_id, chat_name FROM Chats WHERE chat_id = " +
                std::to_string(chat_id);
            for (auto [id, chat_name] :
                 W.stream<std::size_t, std::string>(query)) {
                j = json{{"chat_id", chat_id}, {"chat_name", chat_name}};
            }
            return j.dump();
        } catch (const std::exception &e) {
            server_log << error << e.what() << std::endl;
            return fail;
        }
    }

    std::string get_chat_id(const std::string &chat_name) {
        pqxx::work W(C);
        try {
            json j;
            std::string query =
                "SELECT chat_id, chat_name FROM Chats WHERE chat_name = '" +
                chat_name + "'";
            for (auto [chat_id, name] :
                 W.stream<std::size_t, std::string>(query)) {
                j = json{{"chat_id", chat_id}, {"chat_name", chat_name}};
            }
            return j.dump();
        } catch (const std::exception &e) {
            server_log << error << e.what() << std::endl;
            return fail;
        }
    }

    std::string get_chat_list(std::size_t user_id) {
        pqxx::work W(C);
        try {
            json j;
            std::string query =
                "SELECT user_id, chat_id FROM UserID_ChatID WHERE user_id = " +
                std::to_string(user_id);
            for (auto [id, chat_id] :
                 W.stream<std::size_t, std::size_t>(query)) {
                j.push_back(json{{"chat_id", chat_id}});
            }
            return j.dump();
        } catch (const std::exception &e) {
            server_log << error << e.what() << std::endl;
            return fail;
        }
    }

    std::string set_trello_token(std::size_t user_id,
                                 const std::string &trello_token) {
        pqxx::work W(C);
        try {
            std::string insert_into =
                "INSERT INTO UserID_TrelloToken (user_id, trello_token) Values "
                "(" +
                std::to_string(user_id) + ", " + trello_token + ")";
            W.exec(insert_into);
            W.commit();
            return success;
        } catch (const std::exception &e) {
            server_log << error << e.what() << std::endl;
            return fail;
        }
    }
};

typedef websocketpp::server<websocketpp::config::asio> server;
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
typedef server::message_ptr message_ptr;

void on_http(server *s, const websocketpp::connection_hdl &hdl) {
    server::connection_ptr con = s->get_con_from_hdl(hdl);

    std::string res = con->get_request_body();

    std::stringstream ss;
    ss << "got HTTP request with " << res.size() << " bytes of body data.";
    server_log << request << "got HTTP request with " << res.size()
               << " bytes of body data.";

    con->set_body(ss.str());
    con->set_status(websocketpp::http::status_code::ok);
}

void on_fail(server *s, const websocketpp::connection_hdl &hdl) {
    websocketpp::server<websocketpp::config::asio>::connection_ptr con =
        s->get_con_from_hdl(hdl);
    websocketpp::lib::error_code ec = con->get_ec();
    if (ec) {
        server_log << error << "Connection attempt by client failed because: "
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
    server_log << request << "on_message called with hdl: " << hdl.lock().get()
               << " and message: " << msg->get_payload() << std::endl;

    std::string ECHO_CMD = "echo";
    std::string FIND_USER = "find_user";
    std::string ADD_USER = "add_user";
    std::string ADD_CHAT = "add_chat";
    std::string ADD_MESSAGE = "add_message";
    std::string LINK_USER_TO_CHAT = "link_user_to_chat";
    std::string GET_CHAT_NAME = "get_chat_name";
    std::string GET_CHAT_ID = "get_chat_id";
    std::string GET_CHAT_HISTORY = "get_chat_history";
    std::string GET_CHAT_LIST = "get_chat_list";
    std::string SET_TRELLO_TOKEN = "set_trello_token";

    try {
        if (msg->get_payload().substr(0, ECHO_CMD.size()) == "echo") {
            s->send(hdl, msg->get_payload().substr(ECHO_CMD.size() + 1),
                    msg->get_opcode());
        } else if (msg->get_payload().substr(0, FIND_USER.size()) ==
                   "find_user") {
            std::string user_name =
                msg->get_payload().substr(FIND_USER.size() + 1);
            std::string message = database->find_user(user_name);
            s->send(hdl, message, msg->get_opcode());
        } else if (msg->get_payload().substr(0, ADD_USER.size()) ==
                   "add_user") {
            std::string user_name =
                msg->get_payload().substr(ADD_USER.size() + 1);
            std::string message = database->add_user(user_name);
            s->send(hdl, message, msg->get_opcode());
        } else if (msg->get_payload().substr(0, ADD_CHAT.size()) ==
                   "add_chat") {
            std::string chat_name =
                msg->get_payload().substr(ADD_CHAT.size() + 1);
            std::string message = database->add_chat(chat_name);
            s->send(hdl, message, msg->get_opcode());
        } else if (msg->get_payload().substr(0, ADD_MESSAGE.size()) ==
                   "add_message") {
            std::stringstream ss(
                msg->get_payload().substr(ADD_MESSAGE.size() + 1));
            std::size_t chat_id, sender_id;
            ss >> chat_id >> sender_id;
            ss.ignore(1);
            std::string message_text(ss.str().substr(ss.tellg()));
            std::string message =
                database->add_message(chat_id, sender_id, message_text);
            s->send(hdl, message, msg->get_opcode());
        } else if (msg->get_payload().substr(0, LINK_USER_TO_CHAT.size()) ==
                   "link_user_to_chat") {
            std::stringstream ss(
                msg->get_payload().substr(LINK_USER_TO_CHAT.size() + 1));
            std::size_t user_id, chat_id;
            ss >> user_id >> chat_id;
            std::string message = database->link_user_to_chat(user_id, chat_id);
            s->send(hdl, message, msg->get_opcode());
        } else if (msg->get_payload().substr(0, GET_CHAT_NAME.size()) ==
                   "get_chat_name") {
            std::size_t chat_id =
                std::stoi(msg->get_payload().substr(GET_CHAT_NAME.size() + 1));
            std::string message = database->get_chat_name(chat_id);
            s->send(hdl, message, msg->get_opcode());
        } else if (msg->get_payload().substr(0, GET_CHAT_ID.size()) ==
                   "get_chat_id") {
            std::string chat_name =
                msg->get_payload().substr(GET_CHAT_ID.size() + 1);
            std::string message = database->get_chat_id(chat_name);
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
        } else if (msg->get_payload().substr(0, SET_TRELLO_TOKEN.size()) ==
                   "set_trello_token") {
            std::stringstream ss(
                msg->get_payload().substr(SET_TRELLO_TOKEN.size() + 1));
            std::size_t user_id;
            ss >> user_id;
            ss.ignore(1);
            std::string trello_token;
            std::getline(ss, trello_token);
            std::string message =
                database->set_trello_token(user_id, trello_token);
            s->send(hdl, message, msg->get_opcode());
        }
    } catch (websocketpp::exception const &e) {
        server_log << error << "Send failed because: "
                   << "(" << e.what() << ")" << std::endl;
    }
}

int main() {
    try {
        Database database(pqxx::connection(
            "user=postgres password=12345"));  //пока подключение к localhost
        if (!server_log) {
            std::cerr << "Couldn't open server_log.txt, check if file exists"
                      << std::endl;
            return 1;
        }
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
        server_log << error << e.what() << std::endl;
    } catch (const std::exception &e) {
        server_log << error << e.what() << std::endl;
    } catch (...) {
        server_log << error << "Caught another exception" << std::endl;
    }
    server_log.close();
    return 0;
}