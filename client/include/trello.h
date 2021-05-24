#ifndef TRELLO_H
#define TRELLO_H

#include <QString>
#include <string>
#include <vector>
#include <user.h>
#include <nlohmann/json.hpp>

class Trello {
    //DO NOT CHANGE NEXT LINE. ONLY khram CAN!
    const QString API_KEY = "56ed854395b292ea83d60e4c393a1417";

public:
    Trello() = default;

    ~Trello() = default;

    [[nodiscard]] QString get_api_key() const;
};

#endif //TRELLO_H
