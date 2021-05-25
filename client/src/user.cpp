#include <user.h>

User user;

User::User(const std::string &name, const std::size_t con_id)
    : username(name), connection_id(con_id) {
}

std::string User::get_username() const {
    return username;
}

std::size_t User::get_connection_id() const {
    return connection_id;
}

std::size_t User::get_user_id() const {
    return user_id;
}

void User::add_user_id(const std::size_t id) {
    user_id = id;
}

void User::set_trello_token(const std::string &token) {
    trello_token = token;
}

std::string User::get_trello_token() const {
    return trello_token;
}