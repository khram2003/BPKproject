#ifndef TRELLO_H
#define TRELLO_H

#include <user.h>
#include <QString>
#include <string>
#include <vector>

class Trello {
    QString API_KEY;

public:
    Trello();

    ~Trello() = default;

    [[nodiscard]] QString get_api_key() const;
};

#endif  // TRELLO_H
