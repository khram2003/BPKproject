#include "trello_auth.h"
#include <auth.h>
#include <mainwindow.h>
#include <socket.h>
#include <trello.h>
#include <ui_trello_auth.h>
#include <user.h>
#include <QDesktopServices>
#include <QPushButton>
#include <QUrl>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

trello_auth::trello_auth(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::trello_auth) {
    ui->setupUi(this);

    Trello trello;

    connect(ui->TrelloButton, &QPushButton::clicked, [trello] {
        QString req =
            "https://trello.com/1/"
            "authorize?expiration=never&name=MyPersonalToken&scope=read&"
            "response_type=token&key=" +
            trello.get_api_key();
        QDesktopServices::openUrl(QUrl(req, QUrl::TolerantMode));
    });

    connect(ui->TokenButton, &QPushButton::clicked, [this, trello]() mutable {
        user.set_trello_token((ui->lineEdit_2->text()).toStdString());
        // TODO
        std::string response = endpoint.send_blocking(
            "set_trello_token " + std::to_string(user.get_user_id()) + " " +
            user.get_trello_token());
        ui->lineEdit_2->setText("");
        hide();
        messWindow = new MainWindow();
        messWindow->show();
        this->close();
    });

    this->setFixedSize(400, 400);
}

trello_auth::~trello_auth() {
    delete ui;
}
