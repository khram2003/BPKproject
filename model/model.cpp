#include "model.h"
#include <date.h>
#include <chrono>
#include <iostream>
#include <pqxx/pqxx>
#include <string>

namespace model {
date::hh_mm_ss<std::chrono::system_clock::duration>
Message::get_message_time() const {
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
}  // namespace model
