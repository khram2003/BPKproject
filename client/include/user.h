#ifndef USER_H
#define USER_H

#include <string>

class User {
private:
    std::string username;

public:
    User() = default;

    User(std::string name);

    [[nodiscard]] std::string get_username() const;
};

extern User user;

#endif //USER_H
