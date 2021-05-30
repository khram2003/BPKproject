#ifndef CHAT_VIEW_H
#define CHAT_VIEW_H

#include <QLabel>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

class ChatView : public QWidget {
public:
    ChatView(const QString &name);
    QVBoxLayout *row;
    QLabel *lbl;
};

#endif  // CHAT_VIEW_H
