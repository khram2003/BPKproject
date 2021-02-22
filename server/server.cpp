#include <iostream>
#include <model.h>

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
    std::cout << "  Sent on: " << weekday{date} << ' ' << date.month() << ' ' << date.day() << ' ' << time << ' ' << date.year() << std::endl;
    std::cout << "Server started" << std::endl;
    return 0;
}