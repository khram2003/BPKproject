#include <user.h>

#include <utility>

User user;

User::User(std::string name, std::size_t con_id) : username(std::move(name)), connection_id(con_id) {

}

std::string User::get_username() const {
    return username;
}

std::size_t User::get_connection_id() const {
    return connection_id;
}