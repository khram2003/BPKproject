#include <add_chat.h>
#include <chat_view.h>
#include <mainwindow.h>
#include <socket.h>
#include <ui_add_chat.h>
#include <user.h>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

add_chat::add_chat(QWidget *parent, MainWindow *messWin)
    : QMainWindow(parent), ui(new Ui::add_chat), mess(messWin) {
    ui->setupUi(this);

    connect(ui->AddButton, &QPushButton::clicked, [this] {
        std::string chat_name = ui->lineEdit->text().toStdString();
        std::string chat_exists = "FAIL";
        // TODO make pop up window
        while (chat_exists != "SUCCESS") {
            // TODO FIX INF LOOP
            chat_exists = endpoint.send_blocking("add_chat " + chat_name);
        }
        std::string response =
            endpoint.send_blocking("get_chat_id " + chat_name);
        // todo check fail
        assert(response != "FAIL");
        json j = json::parse(response);
        response = endpoint.send_blocking("link_user_to_chat " +
                                          std::to_string(user.get_user_id()) +
                                          " " + j["chat_id"].dump());
        // todo check fail
        assert(response != "FAIL");
        mess->get_ui()->listWidget_2->clear();
        mess->update_chats();
        hide();
        this->close();
    });

    this->setFixedSize(400, 400);
}

add_chat::~add_chat() {
    delete ui;
}