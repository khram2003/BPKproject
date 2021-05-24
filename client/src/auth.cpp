#include <auth.h>
#include <socket.h>
#include <user.h>
#include <QDesktopServices>
#include <QUrl>
#include <nlohmann/json.hpp>
#include <mainwindow.h>
#include <ui_auth.h>
#include <trello.h>

using json = nlohmann::json;

auth::auth(QWidget *parent) : QMainWindow(parent), ui(new Ui::auth) {
    ui->setupUi(this);

    Trello trello;

    connect(ui->AuthButton, &QPushButton::clicked, [this] {
        if ((ui->lineEdit->text()).size() != 0) {
            user = User(ui->lineEdit->text().toStdString(), 0);
            endpoint.p = std::promise<std::string>();
            endpoint.send("find_user " + user.get_username());
            auto future = endpoint.p.get_future();
            future.wait();
            std::string response = future.get();
            bool new_user = false;
            if (response == "User not found") {
                new_user = true;
                endpoint.send("add_user " + user.get_username());
                endpoint.p = std::promise<std::string>();
                endpoint.send("find_user " + user.get_username());
                future = endpoint.p.get_future();
                future.wait();
                response = future.get();
            }
            auto j = json::parse(response);
            user.add_user_id(j["user_id"]);
            if (new_user) {
                //Adding first chat
                std::string first_chat = user.get_username() + " notes";
                endpoint.send("add_chat " + first_chat);
                endpoint.p = std::promise<std::string>();
                endpoint.send("get_chat_id " + first_chat);
                auto future = endpoint.p.get_future();
                future.wait();
                std::string response = future.get();
                json j = json::parse(response);
                endpoint.send("link_user_to_chat " + std::to_string(user.get_user_id()) + " " + j["chat_id"].dump());
            }
            ui->lineEdit->setText("");
            hide();
            messWindow = new MainWindow();
            messWindow->show();
            this->close();
        }
    });

    connect(ui->TrelloButton, &QPushButton::clicked, [this, trello] {
        QString req =
                "https://trello.com/1/"
                "authorize?expiration=1day&name=MyPersonalToken&scope=read&"
                "response_type=token&key=" +
                trello.get_api_key();
        QDesktopServices::openUrl(QUrl(req, QUrl::TolerantMode));
        user.set_trello_token((ui->lineEdit_2->text()).toStdString());
        //std::cout << user.get_trello_token();
    });

    this->setFixedSize(400, 400);
}


auth::~auth() {
    delete ui;
}
