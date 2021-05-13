#ifndef SOCKET_H
#define SOCKET_H

#include <websocketpp/client.hpp>
#include <unordered_map>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <future>

class connection_metadata {
private:
    std::size_t m_id;
    websocketpp::connection_hdl m_hdl;
    std::string m_status;
    std::string m_uri;
    std::string m_server;
    std::string m_error_reason;

public:
    using ptr = websocketpp::lib::shared_ptr<connection_metadata>;
};

class websocket_endpoint {
private:
    using con_list = std::map<std::size_t, connection_metadata::ptr>;

    websocketpp::client<websocketpp::config::asio_client> m_endpoint;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;

    con_list m_connection_list;
    std::size_t m_next_id;

public:
    std::unordered_map<std::string, std::size_t> username_connection_id;

    std::promise<std::string> p;

    websocket_endpoint();

    std::size_t connect(std::string const &uri);

    void send(std::size_t id, const std::string &message);
};

extern websocket_endpoint endpoint;

#endif //SOCKET_H
