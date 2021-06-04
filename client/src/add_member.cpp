#include <add_member.h>
#include <chat_view.h>
#include <curl_raii.h>
#include <mainwindow.h>
#include <popup.h>
#include <socket.h>
#include <trello.h>
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
    this->setStyleSheet(
        ".QPushButton {background-color : #191919;border-width: 2px; "
        "border-radius: 10px; padding: 6px;}");
    ui->lineEdit->setStyleSheet(
        ".QLineEdit {background-color : #191919;border-width: 2px; "
        "border-radius: 10px; padding: 6px;}");

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
            std::string chatter_id = j["user_id"].dump();
            response = endpoint.send_blocking(
                "link_user_to_chat " + j["user_id"].dump() + " " +
                std::to_string(mess->get_current_chat_id()));
            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }

            response = endpoint.send_blocking(
                "get_trello_token " + std::to_string(user.get_user_id()));
            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }
            j = json::parse(response);
            std::string user_trello_token = j["trello_token"];

            response = endpoint.send_blocking("get_trello_token " + chatter_id);
            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }
            j = json::parse(response);

            std::string chatter_token = j["trello_token"];

            std::string req =
                "https://api.trello.com/1/tokens/" + user_trello_token +
                "/member?key=" + trello.get_api_key().toStdString() +
                "&token=" + user_trello_token;
            curl_raii::curl crl;
            crl.set_url(req);
            std::stringstream in;
            crl.save(in);
            std::string json_string = in.str();
            j = json::parse(json_string);
            auto trello_user_id = j["id"].get<std::string>();

            response = endpoint.send_blocking(
                "get_board_id " + std::to_string(mess->get_current_chat_id()));
            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }

            //            std::cout<<'\n'<<json_string<<'\n';
            j = json::parse(response);
            std::string board_id = j["board_id"];

            std::string chatter_trello_id;
            req = "https://api.trello.com/1/tokens/" + chatter_token +
                  "/member?key=" + trello.get_api_key().toStdString() +
                  "&token=" + chatter_token;

            crl.set_url(req);
            std::stringstream in1;
            crl.save(in1);
            json_string = in1.str();
            j = json::parse(json_string);
            chatter_trello_id = j["id"].get<std::string>();

            //            std::cout<<'\n'<<req<<'\n';
            req = "https://api.trello.com/1/boards/" + board_id + "/members/" +
                  chatter_trello_id +
                  "?key=" + trello.get_api_key().toStdString() +
                  "&token=" + user_trello_token + "&type=admin";
            curl_raii::curl send_curl;
            send_curl.set_url(req);
            send_curl.upload_mode();
            send_curl.send();
            hide();
            this->close();
        }
    });

    this->setFixedSize(400, 400);
}

add_member::~add_member() {
    delete ui;
}
