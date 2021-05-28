#include <add_member.h>
#include <chat_view.h>
#include <mainwindow.h>
#include <popup.h>
#include <socket.h>
#include <ui_add_member.h>
#include <user.h>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

add_member::add_member(QWidget *parent, MainWindow *messWin)
    : QMainWindow(parent), ui(new Ui::add_member), mess(messWin) {
    ui->setupUi(this);

    this->setWindowTitle("Add member to chat");
    this->setStyleSheet(".QPushButton {background-color : #191919;border-width: 2px; border-radius: 10px; padding: 6px;}");
    ui->lineEdit->setStyleSheet(".QLineEdit {background-color : #191919;border-width: 2px; border-radius: 10px; padding: 6px;}");


    connect(ui->AddButton, &QPushButton::clicked, [this] {
        std::string user_name = ui->lineEdit->text().toStdString();
        std::string user_exists;
        user_exists = endpoint.send_blocking("find_user " + user_name);
        if (user_exists == "User not found") {
            up = new PopUp();
            ui->lineEdit->clear();
            up->setPopupText("There is no such user.");
            up->show();
        } else {
            json j = json::parse(user_exists);
            std::string response;
            response = endpoint.send_blocking(
                "link_user_to_chat " + j["user_id"].dump() + " " +
                std::to_string(mess->get_current_chat_id()));
            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }
            hide();
            this->close();
        }
    });

    this->setFixedSize(400, 400);
}

add_member::~add_member() {
    delete ui;
}
