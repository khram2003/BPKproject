#include "auth.h"
#include <socket.h>
#include <user.h>
#include <nlohmann/json.hpp>
#include "mainwindow.h"
#include "ui_auth.h"

using json = nlohmann::json;

auth::auth(QWidget *parent) : QMainWindow(parent), ui(new Ui::auth) {
    ui->setupUi(this);

    messWindow = new MainWindow();

    connect(messWindow, &MainWindow::messWin, [this] {
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
    connect(messWindow, &MainWindow::authWin, this, &auth::show);
    this->setFixedSize(400, 400);
}

void auth::on_AuthButton_clicked() {
    if ((ui->lineEdit->text()).size() != 0) {
        messWindow->show();
        this->close();
    }
}

auth::~auth() {
    delete ui;
}
