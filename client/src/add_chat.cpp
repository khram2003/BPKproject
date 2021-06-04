#include <add_chat.h>
#include <chat_view.h>
#include <curl_raii.h>
#include <mainwindow.h>
#include <popup.h>
#include <socket.h>
#include <trello.h>
#include <ui_add_chat.h>
#include <user.h>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
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
            std::string response = endpoint.send_blocking(
                "get_trello_token " + std::to_string(user.get_user_id()));
            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }
            json j = json::parse(response);
            std::string user_trello_token = j["trello_token"];

            std::string req = "https://api.trello.com/1/boards/?key=" +
                              trello.get_api_key().toStdString() +
                              "&token=" + user_trello_token;
            curl_raii::curl crl1;
            crl1.set_url(req);
            crl1.post_mode("&name=" + chat_name);
            crl1.send();

            req = "https://api.trello.com/1/tokens/" + user_trello_token +
                  "/member?key=" + trello.get_api_key().toStdString() +
                  "&token=" + user_trello_token;
            std::cout << req << '\n';
            curl_raii::curl crl;
            crl.set_url(req);
            std::stringstream in;
            crl.save(in);
            std::string json_string = in.str();
            j = json::parse(json_string);
            auto trello_user_id = j["id"].get<std::string>();

            req = "https://api.trello.com/1/members/" + trello_user_id +
                  "/boards?key=" + trello.get_api_key().toStdString() +
                  "&token=" + user_trello_token;
            crl.set_url(req);
            std::stringstream in1;
            crl.save(in1);
            std::cout << "qwertyuiokjhgc";
            json_string = in1.str();
            j = json::parse(json_string);
            std::string board_id;
            for (auto board : j) {
                if (board["name"] == chat_name) {
                    board_id = board["id"];
                }
            }
            response = endpoint.send_blocking("get_chat_id " + chat_name);
            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }
            j = json::parse(response);
            response = endpoint.send_blocking(
                "set_board_id " + j["chat_id"].dump() + " " + board_id);
            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }
            //            j = json::parse(response);
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