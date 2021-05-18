#ifndef SOCKET_H
#define SOCKET_H

#include <websocketpp/client.hpp>
#include <unordered_map>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <future>

//using client = websocketpp::client<websocketpp::config::asio_client>;
//using message_ptr = websocketpp::config::asio_client::message_type::ptr;

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

    void on_message(websocketpp::connection_hdl hdl,
                    websocketpp::config::asio_client::message_type::ptr msg);

    void on_open(websocketpp::client<websocketpp::config::asio_client> *c, websocketpp::connection_hdl hdl);

    void on_fail(websocketpp::client<websocketpp::config::asio_client> *c, websocketpp::connection_hdl hdl);

    void on_close(websocketpp::client<websocketpp::config::asio_client> *c, websocketpp::connection_hdl hdl);

    [[nodiscard]] websocketpp::connection_hdl get_hdl() const;

    [[nodiscard]] std::size_t get_id() const;

    [[nodiscard]] std::string get_status() const;

    connection_metadata(std::size_t id,
                        websocketpp::connection_hdl hdl,
                        std::string uri);
};

class websocket_endpoint {
private:
    using con_list = std::unordered_map<std::size_t, connection_metadata::ptr>;

    websocketpp::client<websocketpp::config::asio_client> m_endpoint;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
    con_list m_connection_list;

    const std::string uri = "ws://localhost:9002";

public:
    std::promise<std::string> p;

    websocket_endpoint();

    void init_factory();

    std::size_t connect();

    void send(std::size_t id, const std::string &message);

    connection_metadata::ptr get_metadata(std::size_t id) const;

    ~websocket_endpoint();
};

extern websocket_endpoint endpoint;

#endif //SOCKET_H
