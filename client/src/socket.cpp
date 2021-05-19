#include <socket.h>
#include <websocketpp/client.hpp>
#include <map>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/roles/client_endpoint.hpp>
#include <utility>

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

websocket_endpoint::websocket_endpoint() {
    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

    m_endpoint.init_asio();
    m_endpoint.start_perpetual();

    init_factory();

    m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(
            &client::run, &m_endpoint);
}

void websocket_endpoint::init_factory() {
    websocketpp::lib::error_code ec;
    client::connection_ptr con = m_endpoint.get_connection(uri, ec);
    connection_metadata::ptr metadata_ptr(new connection_metadata(0, con->get_handle(), uri));
    m_connection_list[0] = metadata_ptr;
    con->set_open_handler(websocketpp::lib::bind(
            &connection_metadata::on_open,
            metadata_ptr,
            &m_endpoint,
            websocketpp::lib::placeholders::_1
    ));
    con->set_fail_handler(websocketpp::lib::bind(
            &connection_metadata::on_fail,
            metadata_ptr,
            &m_endpoint,
            websocketpp::lib::placeholders::_1
    ));
    con->set_close_handler(websocketpp::lib::bind(
            &connection_metadata::on_close,
            metadata_ptr,
            &m_endpoint,
            websocketpp::lib::placeholders::_1
    ));
    con->set_message_handler(websocketpp::lib::bind(&connection_metadata::on_message, metadata_ptr,
                                                    websocketpp::lib::placeholders::_1,
                                                    websocketpp::lib::placeholders::_2));
    //todo Вот здесь все норм подключается
    m_endpoint.connect(con);
}

std::size_t websocket_endpoint::connect() {
    websocketpp::lib::error_code ec;

    client::connection_ptr con = m_endpoint.get_connection(uri, ec);

    if (ec) {
        std::cerr << "> Connect initialization error: " << ec.message()
                  << std::endl;
        return 0;
    }

    p = std::promise<std::string>();
    send(0, "get_conn");
    auto future = endpoint.p.get_future();
    future.wait();
    std::size_t new_id = std::stoull(future.get());
    connection_metadata::ptr metadata_ptr(new connection_metadata(new_id, con->get_handle(), uri));
    m_connection_list[new_id] = metadata_ptr;
    con->set_open_handler(websocketpp::lib::bind(
            &connection_metadata::on_open,
            metadata_ptr,
            &m_endpoint,
            websocketpp::lib::placeholders::_1
    ));
    con->set_fail_handler(websocketpp::lib::bind(
            &connection_metadata::on_fail,
            metadata_ptr,
            &m_endpoint,
            websocketpp::lib::placeholders::_1
    ));
    con->set_close_handler(websocketpp::lib::bind(
            &connection_metadata::on_close,
            metadata_ptr,
            &m_endpoint,
            websocketpp::lib::placeholders::_1
    ));
    con->set_message_handler(websocketpp::lib::bind(&connection_metadata::on_message, metadata_ptr,
                                                    websocketpp::lib::placeholders::_1,
                                                    websocketpp::lib::placeholders::_2));

    //todo Вот здесь статус остается Connecting, а должен стать Open
    m_endpoint.connect(con);

    return new_id;
}

void websocket_endpoint::send(std::size_t id, const std::string &message) {
    websocketpp::lib::error_code ec;

    auto metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
        return;
    }

    m_endpoint.send(metadata_it->second->get_hdl(), message,
                    websocketpp::frame::opcode::text, ec);
    if (ec) {
        return;
    }
}

connection_metadata::ptr websocket_endpoint::get_metadata(std::size_t id) const {
    auto metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
        return connection_metadata::ptr();
    } else {
        return metadata_it->second;
    }
}

websocket_endpoint::~websocket_endpoint() {
    m_endpoint.stop_perpetual();

    for (auto &it : m_connection_list) {
        if (it.second->get_status() != "Open") {
            continue;
        }
        websocketpp::lib::error_code ec;
        if (it.first != 0) {
            m_endpoint.close(it.second->get_hdl(),
                             websocketpp::close::status::going_away, "", ec);
        }
        if (ec) {
            return;
        }
    }

    m_thread->join();
}