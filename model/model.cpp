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

void Database::connect_to_database(pqxx::connection &C) {
    try {
        std::cout << "Connected to " << C.dbname() << std::endl;
    } catch (std::exception const &e) {
        std::cerr << e.what() << '\n';
        return;
    }
}

void Database::create_table(pqxx::connection &C/*, pqxx::result &R*/) {
    pqxx::work W(C);
    try {
        W.exec("CREATE TABLE IF NOT EXISTS accounts (id INT PRIMARY KEY, balance INT)");
        W.exec("INSERT INTO accounts (id, balance) VALUES (1, 1000), (2, 250)");
        W.commit();
        std::cout << "Created table" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
}  // namespace model
