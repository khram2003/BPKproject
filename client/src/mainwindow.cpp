#include <add_chat.h>
#include <add_member.h>
#include <chat_view.h>
#include <clickablelabel.h>
#include <mainwindow.h>
#include <message_view.h>
#include <popup.h>
#include <socket.h>
#include <ui_add_member.h>
#include <ui_mainwindow.h>
#include <user.h>
#include <view_boards.h>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QPixmap>
#include <QPushButton>
#include <QShortcut>
#include <QTimer>
#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>
#include <unordered_map>

using json = nlohmann::json;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    this->setWindowTitle("XINU");
    this->setStyleSheet(".QPushButton {background-color : #191919;border-width: 2px; border-style: solid; border-radius: 10px; padding: 6px;} ");
    ui->textEdit->setStyleSheet(".QTextEdit {background-color : #191919;border-width: 2px; border-radius: 10px; padding: 6px;}");
    ui->listWidget->setStyleSheet(".QListWidget {background-color : #191919;border-width: 2px; border-radius: 10px; padding: 6px;}");
    ui->listWidget_2->setStyleSheet(".QListWidget {background-color : #191919;border-width: 2px; border-radius: 10px; padding: 6px;}");


    update_chats();
    num_of_chats = 0;
    update_chats_ui();
    update_messages(icon_to_chat_id[ui->listWidget_2->item(0)]);
    current_chat_id = icon_to_chat_id[ui->listWidget_2->item(0)];
    ui->listWidget_2->setCurrentRow(current_chat);
    ui->label->setText(icon_to_name[ui->listWidget_2->item(current_chat)]);

    connect(ui->sendButton, &QPushButton::clicked, [this] {
        if ((ui->textEdit->toPlainText()).size() != 0) {
            ui->textEdit->setWordWrapMode(
                QTextOption::WrapAtWordBoundaryOrAnywhere);
            QListWidgetItem *item = new QListWidgetItem;
            MessageViewOut *row =
                new MessageViewOut(ui->textEdit->toPlainText());
            row->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            ui->listWidget->setSizeAdjustPolicy(QListWidget::AdjustToContents);
            ui->listWidget->setWordWrap(true);
            ui->listWidget->addItem(item);
            ui->listWidget->setItemWidget(item, row);
            item->setSizeHint(row->sizeHint());
            item->setFont(QFont("Helvetica [Cronyx]", 12));
            std::size_t chat_id =
                icon_to_chat_id[ui->listWidget_2->item(current_chat)];
            std::string response = endpoint.send_blocking(
                "add_message " + std::to_string(chat_id) + " " +
                std::to_string(user.get_user_id()) + " " +
                ui->textEdit->toPlainText().toStdString());
            if (response == "FAIL") {
                up = new PopUp();
                up->setPopupText(
                    "Oops! Something went wrong... Don't worry that's on us.");
                up->show();
            }
            current_chat_messages_size++;
            ui->textEdit->setPlainText("");
            ui->listWidget->scrollToBottom();
        }
    });

    connect(ui->listWidget_2, SIGNAL(itemClicked(QListWidgetItem *)),
            ui->listWidget,
            SLOT(on_listWidget_2_itemClicked(QListWidgetItem *)));

    //    connect(ui->listWidget_2->item(current_chat), )

    connect(ui->AddChatButton, &QPushButton::clicked, [this] {
        add_chat *chooseWindow = new add_chat(nullptr, this);
        chooseWindow->show();
    });

    connect(ui->memberButton, &QPushButton::clicked, [this] {
        add_member *memberWindow = new add_member(nullptr, this);
        memberWindow->show();
    });

    connect(ui->viewBoardsButton, &QPushButton::clicked, [this] {
        view_boards *viewBrdWindow = new view_boards(nullptr, this);
        viewBrdWindow->show();
    });

    QTimer *timer_chat = new QTimer(this);
    connect(timer_chat, &QTimer::timeout, [this] {
        update_chats();
        if (num_of_chats != size_of_answer) {
            update_chats_ui();
            num_of_chats = ui->listWidget_2->count();
            ui->listWidget_2->setCurrentRow(current_chat);
        }
    });

    timer_chat->start(60);

    QTimer *timer_message = new QTimer(this);
    connect(timer_message, &QTimer::timeout,
            [this] { update_messages(current_chat_id); });

    timer_message->start(60);
}

void MainWindow::update_chats() {
    json chat_ids;
    chat_names.clear();
    std::string response = endpoint.send_blocking(
        "get_chat_list " + std::to_string(user.get_user_id()));
    if (response == "FAIL") {
        up = new PopUp();
        up->setPopupText(
            "Oops! Something went wrong... Don't worry that's on us.");
        up->show();
    }
    chat_ids = json::parse(response);

    for (auto x : chat_ids) {
        response =
            endpoint.send_blocking("get_chat_name " + x["chat_id"].dump());
        if (response == "FAIL") {
            up = new PopUp();
            up->setPopupText(
                "Oops! Something went wrong... Don't worry that's on us.");
            up->show();
        }
        chat_names.push_back(json::parse(response));
    }

    size_of_answer = chat_names.size();
}
void MainWindow::update_messages(std::size_t chat_id) {
    std::string response =
        endpoint.send_blocking("get_chat_history " + std::to_string(chat_id));
    if (response == "FAIL") {
        up = new PopUp();
        up->setPopupText(
            "Oops! Something went wrong... Don't worry that's on us.");
        up->show();
    }
    json messages = json::parse(response);
    if (current_chat_messages_size != messages.size()) {
        ui->listWidget->clear();
        for (auto mess : messages) {
            ui->textEdit->setWordWrapMode(
                QTextOption::WrapAtWordBoundaryOrAnywhere);
            QListWidgetItem *item = new QListWidgetItem;
            std::size_t sender_id = mess["sender_id"];
            if (sender_id == user.get_user_id()) {
                MessageViewOut *row = new MessageViewOut(QString::fromStdString(
                    mess["message_text"].get<std::string>()));
                row->setSizePolicy(QSizePolicy::Preferred,
                                   QSizePolicy::Preferred);
                ui->listWidget->setSizeAdjustPolicy(
                    QListWidget::AdjustToContents);
                ui->listWidget->setWordWrap(true);
                ui->listWidget->addItem(item);
                ui->listWidget->setItemWidget(item, row);
                item->setSizeHint(row->sizeHint());
                item->setFont(QFont("Helvetica [Cronyx]", 12));
            } else {
                response = endpoint.send_blocking("find_user " +
                                                  std::to_string(sender_id));
                if (response == "FAIL") {
                    up = new PopUp();
                    up->setPopupText(
                        "Oops! Something went wrong... Don't worry that's on "
                        "us.");
                    up->show();
                }
                json user_info = json::parse(response);
                MessageViewIn *row = new MessageViewIn(QString::fromStdString(
                    user_info["user_name"].get<std::string>() + ": " +
                    mess["message_text"].get<std::string>()));
                row->setSizePolicy(QSizePolicy::Preferred,
                                   QSizePolicy::Preferred);
                ui->listWidget->setSizeAdjustPolicy(
                    QListWidget::AdjustToContents);
                ui->listWidget->setWordWrap(true);
                ui->listWidget->addItem(item);
                ui->listWidget->setItemWidget(item, row);
                item->setSizeHint(row->sizeHint());
                item->setFont(QFont("Helvetica [Cronyx]", 12));
            }
        }
        ui->listWidget->scrollToBottom();
        current_chat_messages_size = messages.size();
    }
}

Ui::MainWindow *MainWindow::get_ui() const {
    return ui;
}

void MainWindow::on_listWidget_2_itemClicked(QListWidgetItem *item) {
    current_chat_messages_size = 0;
    ui->listWidget->clear();
    current_chat = ui->listWidget_2->currentRow();
    update_messages(icon_to_chat_id[item]);
    current_chat_id = icon_to_chat_id[item];
    ui->label->setText(icon_to_name[ui->listWidget_2->item(current_chat)]);
}

MainWindow::~MainWindow() {
    delete ui;
}
void MainWindow::update_chats_ui() {
    ui->listWidget_2->clear();
    for (auto x : chat_names) {
        QListWidgetItem *item = new QListWidgetItem;
        std::string chat_name = x["chat_name"];
        std::size_t chat_id = x["chat_id"];
        icon_to_chat_id[item] = chat_id;
        icon_to_name[item] = QString::fromStdString(chat_name);
        ChatView *row = new ChatView(QString::fromStdString(chat_name));
        ui->listWidget_2->setWordWrap(true);
        ui->listWidget_2->addItem(item);
        ui->listWidget_2->setItemWidget(item, row);
        item->setSizeHint(row->minimumSizeHint());
        item->setFont(QFont("Helvetica [Cronyx]", 12));
        item->setSizeHint(QSize(2, 52));
    }
}

std::size_t MainWindow::get_current_chat_id() const {
    return current_chat_id;
}
