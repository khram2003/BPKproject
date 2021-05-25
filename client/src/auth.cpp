#include <auth.h>
#include <mainwindow.h>
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

    connect(ui->AuthButton, &QPushButton::clicked, [this, &new_user] {
        if ((ui->lineEdit->text()).size() != 0) {
            // TODO FIX LOGIC IN THIS METHOD ASAP
            user = User(ui->lineEdit->text().toStdString(), 0);
            endpoint.p = std::promise<std::string>();
            endpoint.send("find_user " + user.get_username());
            std::string response = endpoint.update_future();
            assert(response != "FAIL");
            std::string user_exists = "FAIL";
            if (response == "User not found") {
                new_user = true;
                while (user_exists != "SUCCESS") {
                    endpoint.p = std::promise<std::string>();
                    endpoint.send("add_user " + user.get_username());
                    user_exists = endpoint.update_future();
                }
                endpoint.p = std::promise<std::string>();
                endpoint.send("find_user " + user.get_username());
                response = endpoint.update_future();
            }
            auto j = json::parse(response);
            user.add_user_id(j["user_id"]);
            if (new_user) {
                // Adding first chat
                std::string first_chat = user.get_username() + " notes";
                endpoint.p = std::promise<std::string>();
                endpoint.send("add_chat " + first_chat);
                response = endpoint.update_future();
                // TODO check fails
                assert(response != "FAIL");
                endpoint.p = std::promise<std::string>();
                endpoint.send("get_chat_id " + first_chat);
                response = endpoint.update_future();
                assert(response != "FAIL");
                json j = json::parse(response);
                endpoint.p = std::promise<std::string>();
                endpoint.send("link_user_to_chat " +
                              std::to_string(user.get_user_id()) + " " +
                              j["chat_id"].get<std::string>());
                response = endpoint.update_future();
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
