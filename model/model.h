#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <chrono>
#include <date.h>

using namespace date;
using namespace std::chrono;

namespace model {
    class Message {
    private:
        year_month_day message_date = year_month_day{floor<days>(system_clock::now())};
        hh_mm_ss<std::chrono::duration<long long, std::ratio<1, 1000000000>>> message_time = make_time(
                system_clock::now() - floor<days>(system_clock::now()));
        std::size_t sender_id = 0;
        std::size_t recipient_id = 0;
        std::string message_text;

    public:
        Message() = default;

        explicit Message(std::size_t sender_id_, std::size_t recipient_id_, std::string message_text_) : sender_id(
                sender_id_),
                                                                                                         recipient_id(
                                                                                                                 recipient_id_),
                                                                                                         message_text(
                                                                                                                 std::move(
                                                                                                                         message_text_)) {}


        hh_mm_ss<std::chrono::duration<long long, std::ratio<1, 1000000000>>> get_message_time() const;

        year_month_day get_message_date() const;

        std::string get_message_text() const;

        std::size_t get_sender_id() const;

        std::size_t get_recipient_id() const;
    };
}

#endif // MODEL_H
