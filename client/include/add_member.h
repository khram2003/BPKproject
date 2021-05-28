#ifndef ADD_MEMBER_H
#define ADD_MEMBER_H

#include <mainwindow.h>
#include <popup.h>
#include <ui_mainwindow.h>
#include <QMainWindow>

namespace Ui {
class add_member;
}

class add_member : public QMainWindow {
    Q_OBJECT

public:
    explicit add_member(QWidget *parent = nullptr,
                        MainWindow *messWin = nullptr);

    ~add_member() override;

private:
    Ui::add_member *ui;
    PopUp *up;
    MainWindow *mess;
};

#endif  // ADD_MEMBER_H
