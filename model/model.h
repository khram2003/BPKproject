#ifndef MODEL_H
#define MODEL_H

#include <date.h>
#include <chrono>
#include <iostream>

namespace model {

class Message {
private:
    date::year_month_day message_date = date::year_month_day{
        date::floor<date::days>(std::chrono::system_clock::now())};
    date::hh_mm_ss<std::chrono::system_clock::duration>
        message_time = date::make_time(
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
}  // namespace model

#endif  // MODEL_H
