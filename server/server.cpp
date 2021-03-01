#include <model.h>
#include <iostream>
#include <pqxx/pqxx>

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
                std::to_string(chat_id) + ", " + std::to_string(sender_id) + ", " +
                std::to_string(recipient_id) + ", '" + message_text + "')";
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
};

int main() {
    model::Message msg(1, 2, "hello");
    auto time = msg.get_message_time();
    auto date = msg.get_message_date();
    auto sender_id = msg.get_sender_id();
    auto recipient_id = msg.get_recipient_id();
    auto text = msg.get_message_text();
    std::cout << "Message created: " << std::endl;
    std::cout << "  From #" << sender_id << std::endl;
    std::cout << "  To #" << recipient_id << std::endl;
    std::cout << "  Text: " << text << std::endl;
    std::cout << "  Sent on: " << date::weekday{date} << ' ' << date.month()
              << ' ' << date.day() << ' ' << time << ' ' << date.year()
              << std::endl;
    std::cout << "Server started" << std::endl;
    Database database(pqxx::connection("user=postgres password=12345")); //пока подключение к localhost
    std::string chat_name = "first chat";
    std::string user1 = "user 1";
    std::string user2 = "user 2";
    database.add_user(user1); //работает 1 раз, так как у каждого пользователя уникальный ID
    database.add_user(user2);
    database.add_chat(chat_name);
    database.add_message(1, sender_id, recipient_id, text);
    return 0;
}