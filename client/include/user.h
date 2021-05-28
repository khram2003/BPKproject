#ifndef USER_H
#define USER_H

#include <string>

class User {
private:
    std::string username;
    std::string trello_token;
    std::string google_token;
    std::string trello_user_id;
    std::size_t user_id;

public:
    User() = default;

    void set_username(const std::string &name);

    void set_user_id(std::size_t);

    void set_trello_token(const std::string &token);

    void set_google_token(const std::string &token);

    void set_trello_user_id(const std::string &trello_id);

    [[nodiscard]] std::string get_username() const;

    [[nodiscard]] std::size_t get_user_id() const;

    [[nodiscard]] std::string get_trello_user_id();

    [[nodiscard]] std::string get_trello_token() const;

    [[nodiscard]] std::string get_google_token() const;

    void download_userinfo();

    void download_avatar(const std::string &url);
};

extern User user;

#endif  // USER_H
