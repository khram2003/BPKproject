#ifndef MESSAGE_VIEW_H
#define MESSAGE_VIEW_H

#include <QString>
#include <QWidget>

class MessageViewOut : public QWidget {
public:
    [[maybe_unused]] MessageViewOut(const QString &msg);
};

class MessageViewIn : public QWidget {
public:
    MessageViewIn(const QString &msg);
};

#endif  // MESSAGE_VIEW_H
