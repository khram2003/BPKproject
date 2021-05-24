#ifndef ADD_CHAT_H
#define ADD_CHAT_H

#include <QMainWindow>
#include <mainwindow.h>
#include <ui_mainwindow.h>

namespace Ui {
    class add_chat;
}

class add_chat : public QMainWindow {
Q_OBJECT

public:
    explicit add_chat(QWidget *parent = nullptr, MainWindow *messWin = nullptr);

    ~add_chat() override;

private:
    Ui::add_chat *ui;
    MainWindow *mess;
};

#endif //ADD_CHAT_H
