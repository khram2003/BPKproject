#include <add_member.h>
#include <chat_view.h>
#include <mainwindow.h>
#include <popup.h>
#include <socket.h>
#include <ui_add_member.h>
#include <user.h>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

add_member::add_member(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::add_member) {
    ui->setupUi(this);

    connect(ui->AddButton, &QPushButton::clicked, [this] {
        std::string name = ui->lineEdit->text().toStdString();
        std::string user_exists;
        // TODO make pop up window
        user_exists = endpoint.send_blocking("find_user " + name);
        if (user_exists != "User not found") {
            up = new PopUp();
            ui->lineEdit->clear();
            up->setPopupText("There is no such user.");
            up->show();

        } else {
            // TODO sasha
            //          endpoint.p = std::promise<std::string>();
            //          endpoint.send("find_user " + name);
            //          std::string response = endpoint.update_future();
            //          // todo check fail
            //          assert(response != "FAIL");
            //          json j = json::parse(response);
            //          endpoint.p = std::promise<std::string>();
            //          endpoint.send("link_user_to_chat " +
            //                        j["user_id"].dump() + " " +
            //                        ;
            //          response = endpoint.update_future();
            //          // todo check fail
            //          assert(response != "FAIL");
            hide();
            this->close();
        }
    });

    this->setFixedSize(400, 400);
}

add_member::~add_member() {
    delete ui;
}
