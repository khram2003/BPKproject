#include <add_member.h>
#include <chat_view.h>
#include <curl/curl.h>
#include <curl_raii.h>
#include <mainwindow.h>
#include <popup.h>
#include <socket.h>
#include <trello.h>
#include <ui_view_boards.h>
#include <user.h>
#include <view_boards.h>
#include <QDesktopServices>
#include <QMainWindow>
#include <QPushButton>
#include <QUrl>
#include <QWidget>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

view_boards::view_boards(QWidget *parent, MainWindow *messWin)
    : QMainWindow(parent), ui(new Ui::view_boards), mess(messWin) {
    ui->setupUi(this);
    ui->listWidget->setStyleSheet(
        ".QListWidget {background-color : #191919;border-width: 2px; "
        "border-radius: 10px; padding: 6px;}");
    this->setWindowTitle("Boards list");
    std::string response = endpoint.send_blocking(
        "get_trello_token " + std::to_string(mess->chatter));
    if (response == "FAIL") {
        up = new PopUp();
        up->setPopupText(
            "Oops! Something went wrong... Don't worry that's on us.");
        up->show();
    }
    json j = json::parse(response);

    chatter_token = j["trello_token"].get<std::string>();
    std::string req = "https://api.trello.com/1/tokens/" + chatter_token +
                      "/member?key=" + trello.get_api_key().toStdString() +
                      "&token=" + chatter_token;
    curl_raii::curl crl;
    crl.set_url(req);
    std::stringstream in;
    crl.save(in);
    std::string json_string = in.str();
    j = json::parse(json_string);
    chatter_id = j["id"].get<std::string>();

    ui->listWidget->clear();
    req = "https://api.trello.com/1/members/" + chatter_id +
          "/boards?key=" + trello.get_api_key().toStdString() +
          "&token=" + chatter_token;
    std::stringstream in1;
    crl.set_url(req);
    crl.save(in1);
    json_string = in1.str();
    j = json::parse(json_string);

    for (auto board : j) {
        auto *item = new QListWidgetItem;
        auto board_name = board["name"].get<std::string>();
        auto board_id = board["id"].get<std::string>();
        icon_to_board_id[item] = board_id;
        auto *row = new ChatView(QString::fromStdString(board_name));

        ui->listWidget->setWordWrap(true);
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, row);
        item->setSizeHint(row->minimumSizeHint());
        item->setFont(QFont("Helvetica [Cronyx]", 12));
        item->setSizeHint(QSize(2, 52));
    }

    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this,
            SLOT(on_listWidget_itemClicked(QListWidgetItem *)));
}

view_boards::~view_boards() {
    delete ui;
}
void view_boards::on_listWidget_itemClicked(
    [[maybe_unused]] QListWidgetItem *item) {
    std::string response = endpoint.send_blocking(
        "get_trello_token " + std::to_string(user.get_user_id()));
    if (response == "FAIL") {
        up = new PopUp();
        up->setPopupText(
            "Oops! Something went wrong... Don't worry that's on us.");
        up->show();
    }
    json j = json::parse(response);
    std::string user_trello_token = j["trello_token"];
    std::string req = "https://api.trello.com/1/tokens/" + user_trello_token +
                      "/member?key=" + trello.get_api_key().toStdString() +
                      "&token=" + user_trello_token;
//    std::cout << req << '\n';
    curl_raii::curl crl;
    crl.set_url(req);
    std::stringstream in;
    crl.save(in);
    std::string json_string = in.str();
    j = json::parse(json_string);
    auto trello_user_id = j["id"].get<std::string>();

    req = "https://api.trello.com/1/boards/" + icon_to_board_id[item] +
          "/members/" + trello_user_id +
          "?key=" + trello.get_api_key().toStdString() +

          "&token=" + chatter_token + "&type=admin";

//    std::cout << req;
    CURL *curl;
    curl = curl_easy_init();
    json json_struct;
    if (curl != nullptr) {
        curl_easy_setopt(curl, CURLOPT_URL, req.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    req = "https://api.trello.com/1/boards/" + icon_to_board_id[item] +
          "?key=" + trello.get_api_key().toStdString();
//    std::cout << req;
    crl.set_url(req);
    std::stringstream in1;
    crl.save(in1);
    json_string = in1.str();
    j = json::parse(json_string);

             auto board_url = j["url"].get<std::string>();
    std::cout<<board_url;
    QDesktopServices::openUrl(
        QUrl(QString::fromUtf8(board_url.c_str()), QUrl::TolerantMode));
}
