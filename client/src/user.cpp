#include <user.h>

#include <utility>

User user;

User::User(std::string name) : username(std::move(name)) {

}

std::string User::get_username() const {
    return username;
}