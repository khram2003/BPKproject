#include "model.h"
#include <date.h>
#include <chrono>
#include <iostream>
#include <pqxx/pqxx>
#include <string>

namespace model {
date::hh_mm_ss<std::chrono::system_clock::duration> Message::get_message_time()
    const {
    return message_time;
}

date::year_month_day Message::get_message_date() const {
    return message_date;
}

std::string Message::get_message_text() const {
    return message_text;
}

std::size_t Message::get_sender_id() const {
    return sender_id;
}

std::size_t Message::get_recipient_id() const {
    return recipient_id;
}

void Database::add_message(std::size_t chat_id,
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

void Database::add_chat(std::string &chat_name) {
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
void Database::add_user(std::string &user_name) {
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
}  // namespace model
