#include <socket.h>

websocket_endpoint endpoint;

using websocketpp::connection_hdl;
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

using websocketpp::lib::condition_variable;
using websocketpp::lib::lock_guard;
using websocketpp::lib::mutex;
using websocketpp::lib::thread;
using websocketpp::lib::unique_lock;
using client = websocketpp::client<websocketpp::config::asio_client>;

websocket_endpoint::websocket_endpoint() : m_next_id(1) {
    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

    m_endpoint.init_asio();
    m_endpoint.start_perpetual();

    m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(
            &client::run, &m_endpoint);
}

std::size_t websocket_endpoint::connect(const std::string &uri) {
    return 0;
}

void websocket_endpoint::send(std::size_t id, const std::string &message) {

}