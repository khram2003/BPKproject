#include "auth.h"
#include "ui_auth.h"
#include <user.h>
#include <socket.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

auth::auth(QWidget *parent) : QDialog(parent), ui(new Ui::auth) {
    ui->setupUi(this);
    connect(ui->AuthButton, &QPushButton::clicked, [this] {
        if ((ui->lineEdit->text()).size() != 0) {
            user = User(ui->lineEdit->text().toStdString(), 0);
            endpoint.p = std::promise<std::string>();
            endpoint.send("find_user " + user.get_username());
            auto future = endpoint.p.get_future();
            future.wait();
            std::string response = future.get();
            if (response == "User not found") {
                endpoint.send("add_user " + user.get_username());
                endpoint.p = std::promise<std::string>();
                endpoint.send("find_user " + user.get_username());
                future = endpoint.p.get_future();
                future.wait();
                response = future.get();
            }
            auto j = json::parse(response);
            user.add_user_id(j["user_id"]);
            ui->lineEdit->setText("");
            hide();
        }
    });
}

auth::~auth() {
    delete ui;
}
