#include <auth.h>
#include <auth_google.h>
#include <mainwindow.h>
#include <popup.h>
#include <socket.h>
#include <trello.h>
#include <ui_auth.h>
#include <user.h>
#include <QDesktopServices>
#include <QUrl>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

auth::auth(QWidget *parent) : QMainWindow(parent), ui(new Ui::auth) {
    ui->setupUi(this);

    Trello trello;

    bool new_user = false;

    connect(ui->AuthButton, &QPushButton::clicked, [this, new_user]() mutable {
        if ((ui->lineEdit->text()).size() != 0) {
            //        GoogleAuth ga;
            // TODO FIX LOGIC IN THIS METHOD ASAP
            user = User(ui->lineEdit->text().toStdString());
            std::string response =
                endpoint.send_blocking("find_user " + user.get_username());
            assert(response != "FAIL");
            std::string user_exists = "FAIL";
            if (response == "User not found") {
                new_user = true;
                while (user_exists != "SUCCESS") {
                    user_exists = endpoint.send_blocking("add_user " +
                                                         user.get_username());
                }
                response =
                    endpoint.send_blocking("find_user " + user.get_username());
            }
            json j = json::parse(response);
            user.add_user_id(j["user_id"]);
            if (new_user) {
                // Adding first chat
                std::string first_chat = user.get_username() + " notes";
                response = endpoint.send_blocking("add_chat " + first_chat);
                // TODO check fails
                assert(response != "FAIL");
                response = endpoint.send_blocking("get_chat_id " + first_chat);
                assert(response != "FAIL");
                j = json::parse(response);
                endpoint.send_blocking("link_user_to_chat " +
                                       std::to_string(user.get_user_id()) +
                                       " " + j["chat_id"].dump());
                assert(response != "FAIL");
                ui->lineEdit->setText("");
                hide();
                trelloWindow = new trello_auth();
                trelloWindow->show();
                this->close();
            } else {
                ui->lineEdit->setText("");
                hide();
                messWindow = new MainWindow();
                messWindow->show();
                this->close();
            }
        }
    });
}

auth::~auth() {
    delete ui;
}
