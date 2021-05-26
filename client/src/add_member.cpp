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
        std::string user_name = ui->lineEdit->text().toStdString();
        //todo FIX
        std::size_t chat_id = icon_to_name[ui->listWidget_2->item(current_chat)];
        std::cerr << chat_id;
        std::string user_exists;
        // TODO make pop up window
        user_exists = endpoint.send_blocking("find_user " + user_name);
        if (user_exists == "User not found") {
            up = new PopUp();
            ui->lineEdit->clear();
            up->setPopupText("There is no such user.");
            up->show();
        } else {
            json j = json::parse(user_exists);
            std::string response;
            response = endpoint.send_blocking("link_user_to_chat " + j["user_id"].dump() + " " /*+ todo chat_id*/);
            assert(response != "FAIL");
            hide();
            this->close();
        }
    });

    this->setFixedSize(400, 400);
}

add_member::~add_member() {
    delete ui;
}
