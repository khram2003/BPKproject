#include <user.h>

User user;

User::User(const std::string &name) : username(name) {
}

std::string User::get_username() const {
    return username;
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

void User::set_google_token(const std::string &token) {
    google_token = token;
}

std::string User::get_trello_token() const {
    return trello_token;
}

std::string User::get_google_token() const {
    return google_token;
}