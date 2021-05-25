#ifndef TRELLO_H
#define TRELLO_H

#include <user.h>
#include <QString>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class Trello {
    // DO NOT CHANGE NEXT LINE. ONLY khram CAN!
    const QString API_KEY = "56ed854395b292ea83d60e4c393a1417";

public:
    Trello() = default;

    ~Trello() = default;

    [[nodiscard]] QString get_api_key() const;
};

#endif  // TRELLO_H
