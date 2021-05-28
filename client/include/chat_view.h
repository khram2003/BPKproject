#ifndef CHAT_VIEW_H
#define CHAT_VIEW_H

#include <QLabel>
#include <QString>
#include <QWidget>
#include <QVBoxLayout>
#include <clickablelabel.h>

class ChatView : public QWidget {
public:
    ChatView(const QString &name);
    QVBoxLayout *row;
    QLabel *lbl;
};

#endif  // CHAT_VIEW_H
