#include <trello.h>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Trello::Trello() {
//    std::ifstream is("BPK_CLIENT_CONFIG.json");
//    if (!is) {
//        throw std::runtime_error(
//            "Couldn't open BPK_CLIENT_CONFIG, check if file exists\n");
//    }
//    json j = json::parse(is);
//
//    is.close();

//    API_KEY = QString::fromStdString(j["trello_api_key"].get<std::string>());
}


QString Trello::get_api_key() const {
    return API_KEY;
}

