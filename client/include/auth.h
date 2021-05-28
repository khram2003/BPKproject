#ifndef AUTH_H
#define AUTH_H

#include <mainwindow.h>
#include <popup.h>
#include <trello_auth.h>
#include <QMainWindow>

namespace Ui {
class auth;
}

class auth : public QMainWindow {
    Q_OBJECT

public:
    explicit auth(QWidget *parent = nullptr);

    ~auth() override;

private:
    Ui::auth *ui;
    MainWindow *messWindow;
    trello_auth *trelloWindow;
    PopUp *up;
};

#endif  // AUTH_H
