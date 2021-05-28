#include <add_chat.h>
#include <chat_view.h>
#include <mainwindow.h>
#include <popup.h>
#include <socket.h>
#include <ui_add_chat.h>
#include <user.h>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

add_chat::add_chat(QWidget *parent, MainWindow *messWin)
    : QMainWindow(parent), ui(new Ui::add_chat), mess(messWin) {
    ui->setupUi(this);

    this->setWindowTitle("Add chat");
    this->setStyleSheet(
        ".QPushButton {background-color : #191919;border-width: 2px; "
        "border-radius: 10px; padding: 6px;}");
    ui->lineEdit->setStyleSheet(
        ".QLineEdit {background-color : #191919;border-width: 2px; "
        "border-radius: 10px; padding: 6px;}");

    connect(ui->AddButton, &QPushButton::clicked, [this] {
        std::string chat_name = ui->lineEdit->text().toStdString();
        std::string chat_exists =
            endpoint.send_blocking("add_chat " + chat_name);
        if (chat_exists != "SUCCESS") {
            up = new PopUp();
            ui->lineEdit->clear();
            up->setPopupText(
                "This chat name is taken. Please, choose another one.");
            up->show();
        } else {
            std::string response =
                endpoint.send_blocking("get_chat_id " + chat_name);
            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }
            json j = json::parse(response);
            response = endpoint.send_blocking(
                "link_user_to_chat " + std::to_string(user.get_user_id()) +
                " " + j["chat_id"].dump());
            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }
            mess->get_ui()->listWidget_2->clear();
            mess->update_chats();
            hide();
            this->close();
        }
    });

    this->setFixedSize(400, 400);
}

add_chat::~add_chat() {
    delete ui;
}