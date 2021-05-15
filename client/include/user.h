#ifndef USER_H
#define USER_H

#include <string>

class User {
private:
    std::string username;
    std::size_t connection_id;

public:
    User() = default;

    User(std::string name, std::size_t con_id);

    [[nodiscard]] std::string get_username() const;

    [[nodiscard]] std::size_t get_connection_id() const;
};

extern User user;

#endif //USER_H
