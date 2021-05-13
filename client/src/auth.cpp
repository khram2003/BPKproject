#include "auth.h"
#include "ui_auth.h"
#include <user.h>
#include <socket.h>
//#include <QText

auth::auth(QWidget *parent) : QDialog(parent), ui(new Ui::auth) {
    ui->setupUi(this);
    connect(ui->AuthButton, &QPushButton::clicked, [this] {
        if ((ui->lineEdit->text()).size() != 0) {
            user = User(ui->lineEdit->text().toStdString());
            endpoint.username_connection_id[user.get_username()] = endpoint.connect(
                    "ws://srv6.yeputons.net:9002");
            endpoint.p = std::promise<std::string>();
            endpoint.send(1, "echo hello");
            auto future = endpoint.p.get_future();
            future.wait();
            std::string s = future.get();
            ui->lineEdit->setText("");
            hide();
        }
    });
}

auth::~auth() {
    delete ui;
}
