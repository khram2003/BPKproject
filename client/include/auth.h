#ifndef AUTH_H
#define AUTH_H

#include <mainwindow.h>
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
};

#endif  // AUTH_H
