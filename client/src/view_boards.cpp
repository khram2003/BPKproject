#include <view_boards.h>
#include <add_member.h>
#include <chat_view.h>
#include <mainwindow.h>
#include <popup.h>
#include <socket.h>
#include <ui_view_boards.h>
#include <user.h>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

view_boards::view_boards(QWidget *parent, MainWindow *messWin)
    : QMainWindow(parent), ui(new Ui::view_boards), mess(messWin) {
    ui->setupUi(this);
    ui->listWidget->setStyleSheet(".QListWidget {background-color : #191919;border-width: 2px; border-radius: 10px; padding: 6px;}");

    this->setWindowTitle("Boards list");

}

view_boards::~view_boards() {
    delete ui;
}