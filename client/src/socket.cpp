#include <socket.h>
#include <utility>
#include <websocketpp/client.hpp>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/roles/client_endpoint.hpp>

websocket_endpoint endpoint;

using websocketpp::connection_hdl;

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
      m_hdl(std::move(hdl)),
      m_status("Connecting"),
      m_uri(std::move(uri)),
      m_server("N/A") {
}

void connection_metadata::on_message(
    websocketpp::connection_hdl hdl,
    const websocketpp::config::asio_client::message_type::ptr &msg) {
    endpoint.p.set_value(msg->get_payload());
}

void connection_metadata::on_open(client *c, websocketpp::connection_hdl hdl) {
    m_status = "Open";
    client::connection_ptr con = c->get_con_from_hdl(std::move(hdl));
    m_server = con->get_response_header("Server");
}

void connection_metadata::on_fail(client *c, websocketpp::connection_hdl hdl) {
    m_status = "Failed";
    client::connection_ptr con = c->get_con_from_hdl(std::move(hdl));
    m_server = con->get_response_header("Server");
    m_error_reason = con->get_ec().message();
}

void connection_metadata::on_close(client *c, websocketpp::connection_hdl hdl) {
    m_status = "Closed";
    client::connection_ptr con = c->get_con_from_hdl(std::move(hdl));
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

websocket_endpoint::websocket_endpoint() {
    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

    m_endpoint.init_asio();
    m_endpoint.start_perpetual();

    init_connection();

    m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(
        &client::run, &m_endpoint);
}

void websocket_endpoint::init_connection() {
    websocketpp::lib::error_code ec;
    client::connection_ptr con = m_endpoint.get_connection(uri, ec);
    connection_metadata::ptr metadata_ptr(
        new connection_metadata(0, con->get_handle(), uri));
    m_connection = metadata_ptr;
    con->set_open_handler([metadata_ptr, capture0 = &m_endpoint](auto &&PH1) {
        metadata_ptr->on_open(capture0, std::forward<decltype(PH1)>(PH1));
    });
    con->set_fail_handler([metadata_ptr, capture0 = &m_endpoint](auto &&PH1) {
        metadata_ptr->on_fail(capture0, std::forward<decltype(PH1)>(PH1));
    });
    con->set_close_handler([metadata_ptr, capture0 = &m_endpoint](auto &&PH1) {
        metadata_ptr->on_close(capture0, std::forward<decltype(PH1)>(PH1));
    });
    con->set_message_handler([metadata_ptr](auto &&PH1, auto &&PH2) {
        metadata_ptr->on_message(std::forward<decltype(PH1)>(PH1),
                                 std::forward<decltype(PH2)>(PH2));
    });
    m_endpoint.connect(con);
}

void websocket_endpoint::send(const std::string &message) {
    websocketpp::lib::error_code ec;
    m_endpoint.send(m_connection->get_hdl(), message,
                    websocketpp::frame::opcode::text, ec);
    if (ec) {
        return;
    }
}

std::string websocket_endpoint::update_future() {
    auto future = p.get_future();
    future.wait();
    return future.get();
}

websocket_endpoint::~websocket_endpoint() {
    m_endpoint.stop_perpetual();
    websocketpp::lib::error_code ec;
    m_endpoint.close(m_connection->get_hdl(),
                     websocketpp::close::status::going_away, "", ec);
    if (ec) {
        return;
    }
    m_thread->join();
}