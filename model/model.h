#ifndef MODEL_H
#define MODEL_H

#include <date.h>
#include <chrono>
#include <iostream>
#include <pqxx/pqxx>

namespace model {
class Message {
private:
    date::year_month_day message_date = date::year_month_day{
        date::floor<date::days>(std::chrono::system_clock::now())};
    date::hh_mm_ss<std::chrono::system_clock::duration> message_time =
        date::make_time(
            std::chrono::system_clock::now() -
            date::floor<date::days>(std::chrono::system_clock::now()));
    std::size_t sender_id = 0;
    std::size_t recipient_id = 0;
    std::string message_text;

public:
    Message() = default;

    explicit Message(std::size_t sender_id_,
                     std::size_t recipient_id_,
                     std::string message_text_)
        : sender_id(sender_id_),
          recipient_id(recipient_id_),
          message_text(std::move(message_text_)) {
    }

    [[nodiscard]] date::hh_mm_ss<std::chrono::system_clock::duration>
    get_message_time() const;

    [[nodiscard]] date::year_month_day get_message_date() const;

    [[nodiscard]] std::string get_message_text() const;

    [[nodiscard]] std::size_t get_sender_id() const;

    [[nodiscard]] std::size_t get_recipient_id() const;
};

class Database {
private:
    pqxx::connection C;

public:
    Database(pqxx::connection &&C_) : C(std::move(C_)) {
        pqxx::work W(C);
        std::string create_table;
        try {
            create_table =
                "CREATE TABLE IF NOT EXISTS Users (user_id BIGSERIAL NOT NULL "
                "PRIMARY KEY, user_name VARCHAR(50))";
            W.exec(create_table);
        } catch (std::exception const &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
        try {
            create_table =
                "CREATE TABLE IF NOT EXISTS Chats (chat_id BIGSERIAL NOT NULL "
                "PRIMARY KEY, chat_name VARCHAR(50))";
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

    void add_message(std::size_t chat_id, std::size_t sender_id, std::size_t recipient_id, std::string &message_text);

    void add_chat(std::string &chat_name);

    void add_user(std::string &user_name);
};

}  // namespace model

#endif  // MODEL_H
