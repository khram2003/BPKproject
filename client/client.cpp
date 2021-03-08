#include <model.h>
#include <iostream>
#include <string>
#include <websocketpp/client.hpp>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

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

class connection_metadata {
public:
    using ptr = websocketpp::lib::shared_ptr<connection_metadata>;

    connection_metadata(int id,
                        websocketpp::connection_hdl hdl,
                        std::string uri)
        : m_id(id),
          m_hdl(hdl),
          m_status("Connecting"),
          m_uri(uri),
          m_server("N/A") {
    }

    void on_open(client *c, websocketpp::connection_hdl hdl) {
        m_status = "Open";

        client::connection_ptr con = c->get_con_from_hdl(hdl);
        m_server = con->get_response_header("Server");
    }

    void on_fail(client *c, websocketpp::connection_hdl hdl) {
        m_status = "Failed";

        client::connection_ptr con = c->get_con_from_hdl(hdl);
        m_server = con->get_response_header("Server");
        m_error_reason = con->get_ec().message();
    }

    void on_close(client *c, websocketpp::connection_hdl hdl) {
        m_status = "Closed";
        client::connection_ptr con = c->get_con_from_hdl(hdl);
        std::stringstream s;
        s << "close code: " << con->get_remote_close_code() << " ("
          << websocketpp::close::status::get_string(
                 con->get_remote_close_code())
          << "), close reason: " << con->get_remote_close_reason();
        m_error_reason = s.str();
    }

    void on_message(websocketpp::connection_hdl, client::message_ptr msg) {
        if (msg->get_opcode() == websocketpp::frame::opcode::text) {
            m_messages.push_back("<< " + msg->get_payload());
        } else {
            m_messages.push_back(
                "<< " + websocketpp::utility::to_hex(msg->get_payload()));
        }
    }

    [[nodiscard]] websocketpp::connection_hdl get_hdl() const {
        return m_hdl;
    }

    [[nodiscard]] int get_id() const {
        return m_id;
    }

    [[nodiscard]] std::string get_status() const {
        return m_status;
    }

    void record_sent_message(const std::string& message) {
        m_messages.push_back(">> " + message);
    }

    friend std::ostream &operator<<(std::ostream &out,
                                    connection_metadata const &data);

private:
    int m_id;
    websocketpp::connection_hdl m_hdl;
    std::string m_status;
    std::string m_uri;
    std::string m_server;
    std::string m_error_reason;
    std::vector<std::string> m_messages;
};

std::ostream &operator<<(std::ostream &out, connection_metadata const &data) {
    out << "> URI: " << data.m_uri << "\n"
        << "> Status: " << data.m_status << "\n"
        << "> Remote Server: "
        << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
        << "> Error/close reason: "
        << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason) << "\n";
    out << "> Messages Processed: (" << data.m_messages.size() << ") \n";

    std::vector<std::string>::const_iterator it;
    for (it = data.m_messages.begin(); it != data.m_messages.end(); ++it) {
        out << *it << "\n";
    }

    return out;
}

class websocket_endpoint {
public:
    websocket_endpoint() : m_next_id(0) {
        m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
        m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

        m_endpoint.init_asio();
        m_endpoint.start_perpetual();

        m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(
            &client::run, &m_endpoint);
    }

    ~websocket_endpoint() {
        m_endpoint.stop_perpetual();

        for (auto it = m_connection_list.begin();
             it != m_connection_list.end(); ++it) {
            if (it->second->get_status() != "Open") {
                // Only close open connections
                continue;
            }

            std::cout << "> Closing connection " << it->second->get_id()
                      << std::endl;

            websocketpp::lib::error_code ec;
            m_endpoint.close(it->second->get_hdl(),
                             websocketpp::close::status::going_away, "", ec);
            if (ec) {
                std::cout << "> Error closing connection "
                          << it->second->get_id() << ": " << ec.message()
                          << std::endl;
            }
        }

        m_thread->join();
    }

    int connect(std::string const &uri) {
        websocketpp::lib::error_code ec;

        client::connection_ptr con = m_endpoint.get_connection(uri, ec);

        if (ec) {
            std::cout << "> Connect initialization error: " << ec.message()
                      << std::endl;
            return -1;
        }

        int new_id = m_next_id++;
        connection_metadata::ptr metadata_ptr =
            websocketpp::lib::make_shared<connection_metadata>(
                new_id, con->get_handle(), uri);
        m_connection_list[new_id] = metadata_ptr;

        con->set_open_handler(websocketpp::lib::bind(
            &connection_metadata::on_open, metadata_ptr, &m_endpoint,
            websocketpp::lib::placeholders::_1));
        con->set_fail_handler(websocketpp::lib::bind(
            &connection_metadata::on_fail, metadata_ptr, &m_endpoint,
            websocketpp::lib::placeholders::_1));
        con->set_close_handler(websocketpp::lib::bind(
            &connection_metadata::on_close, metadata_ptr, &m_endpoint,
            websocketpp::lib::placeholders::_1));
        con->set_message_handler(websocketpp::lib::bind(
            &connection_metadata::on_message, metadata_ptr,
            websocketpp::lib::placeholders::_1,
            websocketpp::lib::placeholders::_2));

        m_endpoint.connect(con);

        return new_id;
    }

    void close(int id,
               websocketpp::close::status::value code,
               std::string reason) {
        websocketpp::lib::error_code ec;

        auto metadata_it = m_connection_list.find(id);
        if (metadata_it == m_connection_list.end()) {
            std::cout << "> No connection found with id " << id << std::endl;
            return;
        }

        m_endpoint.close(metadata_it->second->get_hdl(), code, reason, ec);
        if (ec) {
            std::cout << "> Error initiating close: " << ec.message()
                      << std::endl;
        }
    }

    void send(int id, const std::string &message) {
        websocketpp::lib::error_code ec;

        auto metadata_it = m_connection_list.find(id);
        if (metadata_it == m_connection_list.end()) {
            std::cout << "> No connection found with id " << id << std::endl;
            return;
        }

        m_endpoint.send(metadata_it->second->get_hdl(), message,
                        websocketpp::frame::opcode::text, ec);
        if (ec) {
            std::cout << "> Error sending message: " << ec.message()
                      << std::endl;
            return;
        }

        metadata_it->second->record_sent_message(message);
    }

    connection_metadata::ptr get_metadata(int id) const {
        auto metadata_it = m_connection_list.find(id);
        if (metadata_it == m_connection_list.end()) {
            return connection_metadata::ptr();
        } else {
            return metadata_it->second;
        }
    }

private:
    using con_list = std::map<int, connection_metadata::ptr>;

    client m_endpoint;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;

    con_list m_connection_list;
    int m_next_id;
};

int main() {
    bool stop = false;
    std::string input;
    websocket_endpoint endpoint;

    while (!stop) {
        std::cout << "Enter Command: ";
        std::getline(std::cin, input);

        if (input == "quit") {
            stop = true;
        } else if (input == "help") {
            std::cout << "\nCommand List:\n"
                      << "connect <ws uri>\n"
                      << "send <connection id> <message>\n"
                      << "close <connection id> [<close code:default=1000>] "
                         "[<close reason>]\n"
                      << "show <connection id>\n"
                      << "help: Display this help text\n"
                      << "quit: Exit the program\n"
                      << std::endl;
        } else if (input.substr(0, 7) == "connect") {
            int id = endpoint.connect(input.substr(8));
            if (id != -1) {
                std::cout << "> Created connection with id " << id << std::endl;
            }
        } else if (input.substr(0, 4) == "send") {
            std::stringstream ss(input);

            std::string cmd;
            int id;
            std::string message;

            ss >> cmd >> id;
            std::getline(ss, message);

            endpoint.send(id, message);
        } else if (input.substr(0, 5) == "close") {
            std::stringstream ss(input);

            std::string cmd;
            int id;
            int close_code = websocketpp::close::status::normal;
            std::string reason;

            ss >> cmd >> id >> close_code;
            std::getline(ss, reason);

            endpoint.close(id, close_code, reason);
        } else if (input.substr(0, 4) == "show") {
            int id = atoi(input.substr(5).c_str());

            connection_metadata::ptr metadata = endpoint.get_metadata(id);
            if (metadata) {
                std::cout << *metadata << std::endl;
            } else {
                std::cout << "> Unknown connection id " << id << std::endl;
            }
        } else {
            std::cout << "> Unrecognized Command" << std::endl;
        }
    }

    return 0;
}