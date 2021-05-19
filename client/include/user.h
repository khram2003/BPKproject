#ifndef USER_H
#define USER_H

#include <string>

class User {
private:
    std::string username;
    std::size_t connection_id;
    std::size_t user_id;

public:
    User() = default;

    User(const std::string &name, std::size_t con_id);

    [[nodiscard]] std::string get_username() const;

    [[nodiscard]] std::size_t get_connection_id() const;

    [[nodiscard]] std::size_t get_user_id() const;

    void add_user_id(std::size_t);
};

extern User user;

#endif //USER_H
