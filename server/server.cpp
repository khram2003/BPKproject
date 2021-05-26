#include <model.h>
#include <fstream>
#include <future>
#include <iostream>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <regex>
#include <utility>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

using json = nlohmann::json;

namespace {
std::ofstream server_request_log("server_request_log.txt", std::ofstream::app);
std::ofstream server_error_log("server_error_log.txt", std::ofstream::app);

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
            std::cerr << error_message << std::endl;
        }
        try {
            create_table =
                "CREATE TABLE IF NOT EXISTS Chats (chat_id BIGSERIAL NOT NULL "
                "PRIMARY KEY, chat_name VARCHAR(50) NOT NULL UNIQUE)";
            W.exec(create_table);
        } catch (std::exception const &e) {
            std::cerr << error_message << std::endl;
        }
        try {
            create_table =
                "CREATE TABLE IF NOT EXISTS UserID_ChatID (user_id BIGINT NOT "
                "NULL, chat_id BIGINT NOT NULL, FOREIGN KEY(user_id) "
                "REFERENCES Users(user_id), FOREIGN KEY(chat_id) "
                "REFERENCES Chats(chat_id))";
            W.exec(create_table);
        } catch (std::exception const &e) {
            std::cerr << error_message << std::endl;
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
            std::cerr << error_message << std::endl;
        }
        try {
            create_table =
                "CREATE TABLE IF NOT EXISTS UserID_TrelloToken (user_id BIGINT "
                "NOT NULL, trello_token TEXT NOT NULL UNIQUE, FOREIGN "
                "KEY(user_id) REFERENCES Users(user_id))";
            W.exec(create_table);
        } catch (std::exception const &e) {
            std::cerr << error_message << std::endl;
        }
        W.commit();
    }

    std::string add_message(std::size_t chat_id,
                            std::size_t sender_id,
                            std::string &message_text) {
        message_text =
            std::regex_replace(message_text, std::regex("\'"), "\'\'");
        std::string insert_into =
            "INSERT INTO Messages (chat_id, sender_id, message_text) "
            "Values (" +
            std::to_string(chat_id) + ", " + std::to_string(sender_id) + ", '" +
            message_text + "')";
        try {
            pqxx::work W(C);
            W.exec(insert_into);
            W.commit();
            return success;
        } catch (std::exception const &e) {
            server_error_log << request << insert_into << std::endl;
            server_error_log << e.what() << std::endl;
            return fail;
        }
    }

    std::string add_chat(std::string &chat_name) {
        chat_name = std::regex_replace(chat_name, std::regex("\'"), "\'\'");
        std::string insert_into =
            "INSERT INTO Chats (chat_name) Values ('" + chat_name + "')";
        try {
            pqxx::work W(C);
            W.exec(insert_into);
            W.commit();
            return success;
        } catch (std::exception const &e) {
            server_error_log << request << insert_into << std::endl;
            server_error_log << e.what() << std::endl;
            return fail;
        }
    }

    std::string add_user(std::string &user_name) {
        user_name = std::regex_replace(user_name, std::regex("\'"), "\'\'");
        std::string insert_into =
            "INSERT INTO Users (user_name) Values ('" + user_name + "')";
        try {
            pqxx::work W(C);
            W.exec(insert_into);
            W.commit();
            return success;
        } catch (std::exception const &e) {
            server_error_log << request << insert_into << std::endl;
            server_error_log << e.what() << std::endl;
            return fail;
        }
    }

    std::string link_user_to_chat(std::size_t user_id, std::size_t chat_id) {
        std::string insert_into =
            "INSERT INTO UserID_ChatID (user_id, chat_id) Values (" +
            std::to_string(user_id) + ", " + std::to_string(chat_id) + ")";
        try {
            pqxx::work W(C);
            W.exec(insert_into);
            W.commit();
            return success;
        } catch (std::exception const &e) {
            server_error_log << request << insert_into << std::endl;
            server_error_log << e.what() << std::endl;
            return fail;
        }
    }

    std::string find_user(std::string &user_name) {
        user_name = std::regex_replace(user_name, std::regex("\'"), "\'\'");
        std::string query =
            "SELECT user_id, user_name FROM Users WHERE user_name = '" +
            user_name + "'";
        try {
            pqxx::work W(C);
            for (auto [id, name] : W.stream<std::size_t, std::string>(query)) {
                return json{{"user_id", id}, {"user_name", name}}.dump();
            }
            return "User not found";
        } catch (const std::exception &e) {
            server_error_log << request << query << std::endl;
            server_error_log << e.what() << std::endl;
            return fail;
        }
    }

    std::string get_chat_history(std::size_t chat_id) {
        std::string query =
            "SELECT chat_id, sender_id, message_text FROM "
            "Messages WHERE chat_id = " +
            std::to_string(chat_id);
        try {
            pqxx::work W(C);
            json j;
            for (auto [id, sender_id, message_text] :
                 W.stream<std::size_t, std::size_t, std::string>(query)) {
                j.push_back(json{{"chat_id", chat_id},
                                 {"sender_id", sender_id},
                                 {"message_text", message_text}});
            }
            return j.dump();
        } catch (const std::exception &e) {
            server_error_log << request << query << std::endl;
            server_error_log << e.what() << std::endl;
            return fail;
        }
    }

    std::string get_chat_name(std::size_t chat_id) {
        std::string query =
            "SELECT chat_id, chat_name FROM Chats WHERE chat_id = " +
            std::to_string(chat_id);
        try {
            pqxx::work W(C);
            json j;
            for (auto [id, chat_name] :
                 W.stream<std::size_t, std::string>(query)) {
                j = json{{"chat_id", chat_id}, {"chat_name", chat_name}};
            }
            return j.dump();
        } catch (const std::exception &e) {
            server_error_log << request << query << std::endl;
            server_error_log << e.what() << std::endl;
            return fail;
        }
    }

    std::string get_chat_id(std::string &chat_name) {
        chat_name = std::regex_replace(chat_name, std::regex("\'"), "\'\'");
        std::string query =
            "SELECT chat_id, chat_name FROM Chats WHERE chat_name = '" +
            chat_name + "'";
        try {
            pqxx::work W(C);
            json j;
            for (auto [chat_id, name] :
                 W.stream<std::size_t, std::string>(query)) {
                j = json{{"chat_id", chat_id}, {"chat_name", chat_name}};
            }
            return j.dump();
        } catch (const std::exception &e) {
            server_error_log << request << query << std::endl;
            server_error_log << e.what() << std::endl;
            return fail;
        }
    }

    std::string get_chat_list(std::size_t user_id) {
        std::string query =
            "SELECT user_id, chat_id FROM UserID_ChatID WHERE user_id = " +
            std::to_string(user_id);
        try {
            pqxx::work W(C);
            json j;
            for (auto [id, chat_id] :
                 W.stream<std::size_t, std::size_t>(query)) {
                j.push_back(json{{"chat_id", chat_id}});
            }
            return j.dump();
        } catch (const std::exception &e) {
            server_error_log << request << query << std::endl;
            server_error_log << e.what() << std::endl;
            return fail;
        }
    }

    std::string set_trello_token(std::size_t user_id,
                                 std::string &trello_token) {
        trello_token =
            std::regex_replace(trello_token, std::regex("\'"), "\'\'");
        std::string insert_into =
            "INSERT INTO UserID_TrelloToken (user_id, trello_token) Values "
            "(" +
            std::to_string(user_id) + ", '" + trello_token + "')";
        try {
            pqxx::work W(C);
            W.exec(insert_into);
            W.commit();
            return success;
        } catch (const std::exception &e) {
            server_error_log << request << insert_into << std::endl;
            server_error_log << e.what() << std::endl;
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
    server_request_log << request << "got HTTP request with " << res.size()
                       << " bytes of body data.";

    con->set_body(ss.str());
    con->set_status(websocketpp::http::status_code::ok);
}

void on_fail(server *s, const websocketpp::connection_hdl &hdl) {
    websocketpp::server<websocketpp::config::asio>::connection_ptr con =
        s->get_con_from_hdl(hdl);
    websocketpp::lib::error_code ec = con->get_ec();
    if (ec) {
        server_error_log << error
                         << "Connection attempt by client failed because: "
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
    server_request_log << request
                       << "on_message called with hdl: " << hdl.lock().get()
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
            std::size_t chat_id = 0;
            std::size_t sender_id = 0;
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
            std::size_t user_id = 0;
            std::size_t chat_id = 0;
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
            std::size_t user_id = 0;
            ss >> user_id;
            ss.ignore(1);
            std::string trello_token;
            std::getline(ss, trello_token);
            std::string message =
                database->set_trello_token(user_id, trello_token);
            s->send(hdl, message, msg->get_opcode());
        }
    } catch (const std::exception &e) {
        server_error_log << request
                         << "on_message called with hdl: " << hdl.lock().get()
                         << " and message: " << msg->get_payload() << std::endl;
        server_error_log << error << e.what() << std::endl;
    }
}

int main() {
    try {
        Database database(pqxx::connection(
            "user=postgres password=12345"));  //пока подключение к localhost
        if (!server_error_log) {
            std::cerr
                << error
                << "Couldn't open server_error_log.txt, check if file exists"
                << std::endl;
            return 1;
        }
        if (!server_request_log) {
            std::cerr
                << error
                << "Couldn't open server_request_log.txt, check if file exists"
                << std::endl;
        }
        server server;
        server.set_access_channels(websocketpp::log::alevel::all);
        server.clear_access_channels(websocketpp::log::alevel::frame_payload);
        server.init_asio();
        server.set_http_handler([capture0 = &server](auto &&PH1) {
            return on_http(capture0, std::forward<decltype(PH1)>(PH1));
        });
        server.set_fail_handler([capture0 = &server](auto &&PH1) {
            return on_fail(capture0, std::forward<decltype(PH1)>(PH1));
        });
        server.set_close_handler([capture0 = &server](auto &&PH1) {
            return on_close(capture0, std::forward<decltype(PH1)>(PH1));
        });
        server.set_message_handler(
            [capture0 = &server, capture1 = &database](auto &&PH1, auto &&PH2) {
                return on_message(capture0, capture1,
                                  std::forward<decltype(PH1)>(PH1),
                                  std::forward<decltype(PH2)>(PH2));
            });
        server.listen(9002);
        std::cout << "Listening on 9002" << std::endl;
        server.start_accept();
        server.run();
    } catch (websocketpp::exception const &e) {
        server_error_log << error << e.what() << std::endl;
    } catch (const std::exception &e) {
        server_error_log << error << e.what() << std::endl;
    } catch (...) {
        server_error_log << error << "Caught another exception" << std::endl;
    }
    server_error_log.close();
    server_request_log.close();
    return 0;
}