#ifndef AUTH_H
#define AUTH_H

#include <QMainWindow>
#include <mainwindow.h>

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

private slots:
    void on_AuthButton_clicked();
};

#endif  // AUTH_H
