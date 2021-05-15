#include "auth.h"
#include "ui_auth.h"
#include <user.h>
#include <socket.h>
//#include <QText

auth::auth(QWidget *parent) : QDialog(parent), ui(new Ui::auth) {
    ui->setupUi(this);
    connect(ui->AuthButton, &QPushButton::clicked, [this] {
        if ((ui->lineEdit->text()).size() != 0) {
            std::size_t con_id = endpoint.connect(
                    "ws://srv6.yeputons.net:9002");
            user = User(ui->lineEdit->text().toStdString(), con_id);
            endpoint.p = std::promise<std::string>();
            endpoint.send(user.get_connection_id(), std::string("echo authorized" + user.get_username()));
            auto future = endpoint.p.get_future();
            future.wait();
            std::string s = future.get();
            std::cout << s << std::endl;
            ui->lineEdit->setText("");
            hide();
        }
    });
}

auth::~auth() {
    delete ui;
}
