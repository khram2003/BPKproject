#include <curl_raii.h>
#include <user.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

User user;

void User::set_username(const std::string &name) {
    username = name;
}

void User::set_user_id(const std::size_t id) {
    user_id = id;
}

void User::set_trello_token(const std::string &token) {
    trello_token = token;
}

void User::set_google_token(const std::string &token) {
    google_token = token;
}

void User::set_trello_user_id(const std::string &trello_id) {
    trello_user_id = trello_id;
}

std::string User::get_trello_token() const {
    return trello_token;
}

std::string User::get_google_token() const {
    return google_token;
}

std::string User::get_trello_user_id() {
    return trello_user_id;
}

std::string User::get_username() const {
    return username;
}

std::size_t User::get_user_id() const {
    return user_id;
}

void User::download_userinfo() {
    curl_raii::curl curl;
    curl.set_url("https://www.googleapis.com/oauth2/v3/userinfo?access_token=" +
                 google_token);
    std::stringstream in;
    curl.save(in);
    json j = json::parse(in.str());
    std::cout << in.str() << std::endl;
    std::cout << j["picture"].get<std::string>() << std::endl;
    user.set_username(j["name"].get<std::string>());
    download_avatar(j["picture"].get<std::string>());
}

void User::download_avatar(const std::string &url) {
    curl_raii::curl curl;
    curl.set_url(url);
    std::ofstream is("Sprites/avatar.jpg", std::ofstream::binary);
    curl.save(is);
}
