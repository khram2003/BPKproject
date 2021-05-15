#include <socket.h>
#include <websocketpp/client.hpp>
#include <unordered_map>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/roles/client_endpoint.hpp>

websocket_endpoint endpoint;

using websocketpp::connection_hdl;
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
//
using websocketpp::lib::condition_variable;
using websocketpp::lib::lock_guard;
using websocketpp::lib::mutex;
using websocketpp::lib::thread;
using websocketpp::lib::unique_lock;
using client = websocketpp::client<websocketpp::config::asio_client>;

connection_metadata::connection_metadata(std::size_t id,
                                         websocketpp::connection_hdl hdl,
                                         std::string uri)
        : m_id(id),
          m_hdl(hdl),
          m_status("Connecting"),
          m_uri(uri),
          m_server("N/A") {
}

void connection_metadata::on_message(websocketpp::connection_hdl hdl,
                                    websocketpp::config::asio_client::message_type::ptr msg) {
    endpoint.p.set_value(msg->get_payload());
}

void connection_metadata::on_open(client *c, websocketpp::connection_hdl hdl) {
    m_status = "Open";
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
}

void connection_metadata::on_fail(client *c, websocketpp::connection_hdl hdl) {
    m_status = "Failed";
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
    m_error_reason = con->get_ec().message();
}

void connection_metadata::on_close(client *c, websocketpp::connection_hdl hdl) {
    m_status = "Closed";
    client::connection_ptr con = c->get_con_from_hdl(hdl);
}

websocketpp::connection_hdl connection_metadata::get_hdl() const {
    return m_hdl;
}

std::size_t connection_metadata::get_id() const {
    return m_id;
}

std::string connection_metadata::get_status() const {
    return m_status;
}

websocket_endpoint::websocket_endpoint() : m_next_id(1) {
    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

    m_endpoint.init_asio();
    m_endpoint.start_perpetual();

    m_thread.reset(new websocketpp::lib::thread(&client::run, &m_endpoint));
}

std::size_t websocket_endpoint::connect(const std::string &uri) {
    websocketpp::lib::error_code ec;

    client::connection_ptr con = m_endpoint.get_connection(uri, ec);

    if (ec) {
        std::cerr << "> Connect initialization error: " << ec.message()
                  << std::endl;
        return 0;
    }

    std::size_t new_id = m_next_id++;
    metadata_ptr =
            websocketpp::lib::make_shared<connection_metadata>(
                    new_id, con->get_handle(), uri);

    con->set_open_handler(bind(&connection_metadata::on_open, metadata_ptr,
                               &m_endpoint,
                               websocketpp::lib::placeholders::_1));
    con->set_fail_handler(bind(&connection_metadata::on_fail, metadata_ptr,
                               &m_endpoint,
                               websocketpp::lib::placeholders::_1));
    con->set_close_handler(bind(&connection_metadata::on_close,
                                metadata_ptr, &m_endpoint,
                                websocketpp::lib::placeholders::_1));
    con->set_message_handler(bind(&connection_metadata::on_message,
                                  metadata_ptr,
                                  websocketpp::lib::placeholders::_1,
                                  websocketpp::lib::placeholders::_2));

    m_endpoint.connect(con);

    return new_id;
}

void websocket_endpoint::send(std::size_t id, const std::string &message) {
    websocketpp::lib::error_code ec;
    m_endpoint.send(metadata_ptr->get_hdl(), message,
                    websocketpp::frame::opcode::text, ec);
    if (ec) {
        std::cerr << "> Error sending message: " << ec.message()
                  << std::endl;
        return;
    }
}

websocket_endpoint::~websocket_endpoint() {
    m_endpoint.stop_perpetual();
//
//        websocketpp::lib::error_code ec;
//        m_endpoint.close(it->second->get_hdl(),
//                         websocketpp::close::status::going_away, "", ec);
//        if (ec) {
//            std::cerr << "> Error closing connection "
//                      << it->second->get_id() << ": " << ec.message()
//                      << std::endl;
//        }
//    }

    m_thread->join();
}