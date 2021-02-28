#include <model.h>
#include <iostream>

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
    model::Database database(pqxx::connection("user=postgres password=12345"));
    std::string chat_name = "first chat";
    std::string user1 = "user 1";
    std::string user2 = "user 2";
    database.add_user(user1);
    database.add_user(user2);
    database.add_chat(chat_name);
    database.add_message(1, sender_id, recipient_id, text);
    //database.add_message(100, sender_id, recipient_id, text);
    return 0;
}