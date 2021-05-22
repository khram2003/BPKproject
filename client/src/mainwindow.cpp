#include "mainwindow.h"
#include <socket.h>
#include <user.h>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QPixmap>
#include <QPushButton>
#include <QShortcut>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include "chat_view.h"
#include "message_view.h"
#include "ui_mainwindow.h"

using json = nlohmann::json;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    json chat_id, chat_names;

    // TODO FIX THIS PART

    if (user.get_user_id() != 0) {
        endpoint.p = std::promise<std::string>();
        endpoint.send("get_chat_list " + std::to_string(user.get_user_id()));
        auto future = endpoint.p.get_future();
        future.wait();
        std::string response = future.get();
        chat_id = json::parse(response);
    }
    for (auto x : chat_id) {
        //        endpoint.p = std::promise<std::string>();
        //        endpoint.send("get_chat_name " + x["chat_id"]);
        //        chat_names.push_back(json::parse(response));
        std::cout << x << std::endl;
    }
    for (auto x : chat_names) {
        std::string chat_list;

        std::vector<std::pair<QString, std::size_t>> chat_names_id = {
            {"Biba", 1},
            {"Boba", 2},
            {"Bobo", 3}};  // just for example, should be downloaded

        for (auto x : chat_names_id) {
            QListWidgetItem *item = new QListWidgetItem;
            ChatView *row = new ChatView(/*x.first*/ "hello");
            ui->listWidget_2->setWordWrap(true);
            ui->listWidget_2->addItem(item);
            ui->listWidget_2->setItemWidget(item, row);
            item->setSizeHint(row->minimumSizeHint());
            item->setFont(QFont("Helvetica [Cronyx]", 12));
            item->setSizeHint(QSize(2, 52));
        }

        connect(ui->sendButton, &QPushButton::clicked, [this] {
            if ((ui->textEdit->toPlainText()).size() != 0) {
                ui->textEdit->setWordWrapMode(
                    QTextOption::WrapAtWordBoundaryOrAnywhere);
                QListWidgetItem *item = new QListWidgetItem;
                MessageViewIn *row =
                    new MessageViewIn(ui->textEdit->toPlainText());
                row->setSizePolicy(QSizePolicy::Preferred,
                                   QSizePolicy::Preferred);
                ui->listWidget->setSizeAdjustPolicy(
                    QListWidget::AdjustToContents);
                ui->listWidget->setWordWrap(true);
                ui->listWidget->addItem(item);
                ui->listWidget->setItemWidget(item, row);
                item->setSizeHint(row->sizeHint());
                item->setFont(QFont("Helvetica [Cronyx]", 12));
                // TODO .send(add_message)
                endpoint.p = std::promise<std::string>();
                endpoint.send("echo " +
                              ui->textEdit->toPlainText().toStdString());
                auto future = endpoint.p.get_future();
                future.wait();
                std::string s = future.get();
                std::cout << s << std::endl;
                ui->textEdit->setPlainText("");
            }
        });

        connect(ui->listWidget_2, SIGNAL(itemClicked(QListWidgetItem *)),
                ui->listWidget,
                SLOT(on_listWidget_2_itemClicked(QListWidgetItem *)));

        this->setFixedSize(1000, 600);
    }
}
MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_listWidget_2_itemClicked(QListWidgetItem *item) {
    ui->listWidget->clear();
}
