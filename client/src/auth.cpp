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

    this->setWindowTitle("Authentifucation");
    bool new_user = false;

    connect(ui->AuthButton, &QPushButton::clicked, [this, new_user]() mutable {
        GoogleAuth auth{};
        auth.work();
        std::string response =
            endpoint.send_blocking("find_user " + user.get_username());
        if (response == "FAIL") {
            up = new PopUp();
            up->setPopupText(
                "Oops! Something went wrong... Don't worry that's on us.");
            up->show();
        }
        std::string user_exists = "FAIL";
        if (response == "User not found") {
            new_user = true;
            while (user_exists != "SUCCESS") {
                user_exists =
                    endpoint.send_blocking("add_user " + user.get_username());
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

            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }
            response = endpoint.send_blocking("get_chat_id " + first_chat);
            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }
            j = json::parse(response);
            endpoint.send_blocking("link_user_to_chat " +
                                   std::to_string(user.get_user_id()) + " " +
                                   j["chat_id"].dump());
            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }

            hide();
            trelloWindow = new trello_auth();
            trelloWindow->show();
            this->close();
        } else {
            hide();
            messWindow = new MainWindow();
            messWindow->show();
            this->close();
        }
    });
}

auth::~auth() {
    delete ui;
}