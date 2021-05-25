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
            endpoint.p = std::promise<std::string>();
            // TODO REQUEST TO CHANGE CHAT NAME BECAUSE NOW ITS INF LOOP
            endpoint.send("add_chat " + chat_name);
            chat_exists = endpoint.update_future();
        }
        endpoint.p = std::promise<std::string>();
        endpoint.send("get_chat_id " + chat_name);
        std::string response = endpoint.update_future();
        // todo check fail
        assert(response != "FAIL");
        json j = json::parse(response);
        endpoint.p = std::promise<std::string>();
        endpoint.send("link_user_to_chat " +
                      std::to_string(user.get_user_id()) + " " +
                      j["chat_id"].dump());
        response = endpoint.update_future();
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