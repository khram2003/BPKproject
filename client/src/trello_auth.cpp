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

    connect(ui->TrelloButton, &QPushButton::clicked, [] {
        QString requ =
            "https://trello.com/1/"
            "authorize?expiration=never&name=MyPersonalToken&scope=read&"
            "response_type=token&key=" +
            trello.get_api_key();
        QDesktopServices::openUrl(QUrl(requ, QUrl::TolerantMode));
    });

    //    QString req = "https://api.trello.com/1/tokens/" +
    //                  QString::fromUtf8(user.get_trello_token().c_str()) +
    //                  "/"
    //                  "member?key=" +
    //                  trello.get_api_key() + "&token=" +
    //                  QString::fromUtf8(user.get_trello_token().c_str());


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

        std::string req =
            "https://api.trello.com/1/tokens/"
            "a02b84e322522fd57e1b295598cf6686802e0619af292dccfec6016b5e0d6911/"
            "member?key=56ed854395b292ea83d60e4c393a1417&token="
            "a02b84e322522fd57e1b295598cf6686802e0619af292dccfec6016b5e0d6911";
        curl_raii::curl crl;
        crl.set_url(req);
        crl.set_progress(1L);
        crl.switch_verbose(1L);
        std::stringstream in;
        crl.save(in);
        std::string json_string=in.str();
        std::cout<<json_string;
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
