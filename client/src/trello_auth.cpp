#include <auth.h>
#include <curl_raii.h>
#include <mainwindow.h>
#include <popup.h>
#include <socket.h>
#include <trello.h>
#include <trello_auth.h>
#include <ui_trello_auth.h>
#include <user.h>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPushButton>
#include <QUrl>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;
Trello trello;
trello_auth::trello_auth(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::trello_auth) {
    ui->setupUi(this);

    this->setWindowTitle("Trello Authentifucation");
    this->setStyleSheet(
        ".QPushButton {background-color : #191919;border-width: 2px; "
        "border-radius: 10px; padding: 6px;}");

    ui->lineEdit_2->setStyleSheet(
        ".QLineEdit {background-color : #191919;border-width: 2px; "
        "border-radius: 10px; padding: 6px;}");
    ui->textBrowser->setStyleSheet(
        ".QTextBrowser {background-color : #191919;border-width: 2px; "
        "border-radius: 10px; padding: 6px;}");

    connect(ui->TrelloButton, &QPushButton::clicked, [] {
        QString requ =
            "https://trello.com/1/"
            "authorize?expiration=never&name=MyPersonalToken&scope=read&"
            "response_type=token&key=" +
            trello.get_api_key() + "&scope=read,write,account";
        QDesktopServices::openUrl(QUrl(requ, QUrl::TolerantMode));
    });

    connect(ui->TokenButton, &QPushButton::clicked, this,
            &trello_auth::trelloButtonClicked);

    this->setFixedSize(400, 400);
}

trello_auth::~trello_auth() {
    delete ui;
}

void trello_auth::trelloButtonClicked() {
    if (ui->lineEdit_2->text().size() != 0) {
        user.set_trello_token((ui->lineEdit_2->text()).toStdString());
        std::string response = endpoint.send_blocking(
            "set_trello_token " + std::to_string(user.get_user_id()) + " " +
            user.get_trello_token());

        std::string req = "https://api.trello.com/1/tokens/" +
                          user.get_trello_token() +
                          "/member?key=" + trello.get_api_key().toStdString() +
                          "&token=" + user.get_trello_token();
        curl_raii::curl crl;
        crl.set_url(req);
        std::stringstream in;
        crl.save(in);
        std::string json_string = in.str();
        json j = json::parse(json_string);
        user.set_trello_user_id(j["id"].get<std::string>());
        ui->lineEdit_2->setText("");
        hide();
        messWindow = new MainWindow();
        messWindow->show();
        this->close();
    } else {
        up = new PopUp();
        up->setPopupText("Trello token field should not be empty.");
        up->show();
    }
}
