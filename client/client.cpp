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

    connection_metadata(std::size_t id,
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

    [[nodiscard]] std::size_t get_id() const {
        return m_id;
    }

    [[nodiscard]] std::string get_status() const {
        return m_status;
    }

    void record_sent_message(const std::string &message) {
        m_messages.push_back(">> " + message);
    }

    friend std::ostream &operator<<(std::ostream &out,
                                    connection_metadata const &data);

private:
    std::size_t m_id;
    websocketpp::connection_hdl m_hdl;
    std::string m_status;
    std::string m_uri;
    std::string m_server;
    std::string m_error_reason;
    std::vector<std::string> m_messages;
};

std::ostream &operator<<(std::ostream &out, connection_metadata const &data) {
    out << std::endl
        << "---------- INFO ----------" << std::endl
        << "> URI: " << data.m_uri << std::endl
        << "> Status: " << data.m_status << std::endl
        << "> Remote Server: "
        << (data.m_server.empty() ? "None Specified" : data.m_server)
        << std::endl
        << "> Error/close reason: "
        << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason)
        << std::endl;
    out << "> Messages Processed: (" << data.m_messages.size() << ")"
        << std::endl;

    std::vector<std::string>::const_iterator it;
    for (it = data.m_messages.begin(); it != data.m_messages.end(); ++it) {
        out << *it << std::endl;
    }
    out << "--------------------------" << std::endl;
    return out;
}

class websocket_endpoint {
public:
    websocket_endpoint() : m_next_id(1) {
        m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
        m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

        m_endpoint.init_asio();
        m_endpoint.start_perpetual();

        m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(
            &client::run, &m_endpoint);
    }

    ~websocket_endpoint() {
        m_endpoint.stop_perpetual();

        for (auto it = m_connection_list.begin(); it != m_connection_list.end();
             ++it) {
            if (it->second->get_status() != "Open") {
                std::cerr << "Connetion is not open" << std::endl;
                continue;
            }

            std::cout << "> Closing connection " << it->second->get_id()
                      << std::endl;

            websocketpp::lib::error_code ec;
            m_endpoint.close(it->second->get_hdl(),
                             websocketpp::close::status::going_away, "", ec);
            if (ec) {
                std::cerr << "> Error closing connection "
                          << it->second->get_id() << ": " << ec.message()
                          << std::endl;
            }
        }

        m_thread->join();
    }

    std::size_t connect(std::string const &uri) {
        websocketpp::lib::error_code ec;

        client::connection_ptr con = m_endpoint.get_connection(uri, ec);

        if (ec) {
            std::cerr << "> Connect initialization error: " << ec.message()
                      << std::endl;
            return 0;
        }

        std::size_t new_id = m_next_id++;
        connection_metadata::ptr metadata_ptr =
            websocketpp::lib::make_shared<connection_metadata>(
                new_id, con->get_handle(), uri);
        m_connection_list[new_id] = metadata_ptr;

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

    void close(std::size_t id,
               websocketpp::close::status::value code,
               std::string reason) {
        websocketpp::lib::error_code ec;

        auto metadata_it = m_connection_list.find(id);
        if (metadata_it == m_connection_list.end()) {
            std::cerr << "> No connection found with id " << id << std::endl;
            return;
        }

        m_endpoint.close(metadata_it->second->get_hdl(), code, reason, ec);
        if (ec) {
            std::cerr << "> Error initiating close: " << ec.message()
                      << std::endl;
        }
    }

    void send(std::size_t id, const std::string &message) {
        websocketpp::lib::error_code ec;

        auto metadata_it = m_connection_list.find(id);
        if (metadata_it == m_connection_list.end()) {
            std::cerr << "> No connection found with id " << id << std::endl;
            return;
        }

        m_endpoint.send(metadata_it->second->get_hdl(), message,
                        websocketpp::frame::opcode::text, ec);
        if (ec) {
            std::cerr << "> Error sending message: " << ec.message()
                      << std::endl;
            return;
        }

        metadata_it->second->record_sent_message(message);
    }

    connection_metadata::ptr get_metadata(std::size_t id) const {
        auto metadata_it = m_connection_list.find(id);
        if (metadata_it == m_connection_list.end()) {
            return connection_metadata::ptr();
        } else {
            return metadata_it->second;
        }
    }

private:
    using con_list = std::map<std::size_t, connection_metadata::ptr>;

    client m_endpoint;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;

    con_list m_connection_list;
    std::size_t m_next_id;
};

int main() {
    bool stop = false;
    std::string command;
    websocket_endpoint endpoint;

    std::cout << "Basic Client by @parseny and @alexbuyan" << std::endl
              << "To see available commands type 'help'" << std::endl;

    while (!stop) {
        std::cout << "> ";
        std::cin >> command;

        if (command == "exit") {
            stop = true;
        } else if (command == "help") {
            std::cout << std::endl
                      << "---------- COMMANDS ----------" << std::endl
                      << "connect (connection_id will be given automatically)" << std::endl
                      << "send <connection id> <message>" << std::endl
                      << "close <connection id> [<close code:default=1000>] "
                         "[<close reason>]"
                      << std::endl
                      << "show_info <connection id>" << std::endl
                      << "help: Show available commands" << std::endl
                      << "exit: Exit the program" << std::endl
                      << "------------------------------" << std::endl
                      << std::endl;
        } else if (command == "connect") {
            std::cout << "> Creating WebSocket..." << std::endl;
            std::size_t connection_id = endpoint.connect("ws://srv6.yeputons.net:9002"); //todo fix this
            if (connection_id != 0) {
                std::cout << "> Created connection with id " << connection_id
                          << std::endl;
            }
        } else if (command == "send") {
            std::size_t id;
            std::string message;
            std::cin >> id;
            std::getline(std::cin, message);
            endpoint.send(id, message.substr(1));
        } else if (command == "close") {
            std::size_t id;
            std::cin >> id;
            std::string parameters;
            std::getline(std::cin, parameters);
            int close_code = websocketpp::close::status::normal;
            std::string reason;
            if (parameters.size() > 1) {
                parameters = parameters.substr(1);
                std::stringstream ss(parameters);
                if (ss >> close_code) {
                    ss >> reason;
                } else {
                    close_code = websocketpp::close::status::normal;
                    reason = parameters;
                }
            }
            std::cout << "> Connection closed with code " << close_code << std::endl;
            endpoint.close(id, close_code, reason);
        } else if (command == "show_info") {
            std::size_t id;
            std::cin >> id;
            connection_metadata::ptr metadata = endpoint.get_metadata(id);
            if (metadata) {
                std::cout << *metadata << std::endl;
            } else {
                std::cerr << "> Unknown connection id " << id << std::endl;
            }
        } else {
            std::cerr << "> Unrecognized Command" << std::endl;
        }
    }
    return 0;
}