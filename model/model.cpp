#include <iostream>
#include <ctime>
#include <string>
#include "model.h"

namespace model {
    tm *Message::utc_time() {
        time_t now = time(0);
        tm *gmtm = gmtime(&now);
        return gmtm;
    }

    tm *Message::get_message_time() {
        return message_time;
    }

    std::string Message::get_message_text() {
        return message_text;
    }

    std::size_t Message::get_sender_id() {
        return sender_id;
    }

    std::size_t Message::get_recipient_id() {
        return recipient_id;
    }
}