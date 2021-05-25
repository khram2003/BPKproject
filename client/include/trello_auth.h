#ifndef TRELLO_AUTH_H
#define TRELLO_AUTH_H

#include <mainwindow.h>
#include <QMainWindow>

namespace Ui {
class trello_auth;
}

class trello_auth : public QMainWindow {
Q_OBJECT

public:
    explicit trello_auth(QWidget *parent = nullptr);

    ~trello_auth() override;

private:
    Ui::trello_auth *ui;
    MainWindow *messWindow;
};

#endif  // TRELLO_AUTH_H
