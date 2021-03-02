#include <WS2tcpip.h>
#include <model.h>
#include <functional>
#include <iostream>
#include "third-party/websocketpp-0.8.2"
#pragma comment(lib, "ws2_32.lib")
#define ASIO_STANDALONE

typedef websocketpp::server<websocketpp::config::asio> server;

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
    
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cout << "Error" << std::endl;
        exit(1);
    }
    SOCKADDR_IN addr;  // хранение адреса
    auto sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1") addr.sin_port = htons(1234);
    addr.sin_family = AF_INET;

    SOCKET sListen;
    sListen =
        socket(AF_INET, SOCK_STREAM,
               NULL);  // исп семейстов интеренет протовколов. 2-й указывает на
                       // протокол, устанавливающий соединение
    bind(sListen, (SOCKADDR *)&addr, sizeof(addr));
    listen(sListen, SOMAXCONN);

    SOCKET newConnection;
    newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

    if(newConnection == 0) {
        std::cout << "Error of connection" << std::endl;
    } else {
        std::cout << "Client connected" << std::endl;
    }
    return 0;
}