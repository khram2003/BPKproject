#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <ctime>

namespace model {
    class Message {
    private:
        tm *utc_time();

        tm *message_time = utc_time();
        std::string message_text;
        std::size_t sender_id = 0;
        std::size_t recipient_id = 0;

    public:
        Message();

        explicit Message(std::size_t sender_id_, std::size_t recipient_id_, std::string message_text_) : sender_id(sender_id_),
                                                                              recipient_id(recipient_id_), message_text(std::move(message_text_)) {}

        tm *get_message_time();

        std::string get_message_text();

        std::size_t get_sender_id();

        std::size_t get_recipient_id();
    };
}

#endif // MODEL_H
