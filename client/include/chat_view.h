#ifndef CHAT_VIEW_H
#define CHAT_VIEW_H

#include <QLabel>
#include <QString>
#include <QWidget>

class ChatView : public QWidget {
public:
    QLabel *lbl;
    ChatView(const QString &name);
    QString get_name() const;
};

#endif  // CHAT_VIEW_H
