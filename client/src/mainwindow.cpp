#include <add_chat.h>
#include <add_member.h>
#include <chat_view.h>
#include <mainwindow.h>
#include <message_view.h>
#include <popup.h>
#include <socket.h>
#include <ui_add_member.h>
#include <ui_mainwindow.h>
#include <user.h>
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

    update_chats();
    num_of_chats = 0;
    update_chats_ui();
    update_messages(icon_to_chat_id[ui->listWidget_2->item(0)]);
    ui->listWidget_2->setCurrentRow(current_chat);
    ui->label->setText(icon_to_name[ui->listWidget_2->item(current_chat)]);

    connect(ui->sendButton, &QPushButton::clicked, [this] {
        if ((ui->textEdit->toPlainText()).size() != 0) {
            ui->textEdit->setWordWrapMode(
                QTextOption::WrapAtWordBoundaryOrAnywhere);
            QListWidgetItem *item = new QListWidgetItem;
            MessageViewIn *row = new MessageViewIn(ui->textEdit->toPlainText());
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
            // TODO check if message is sent
            assert(response != "FAIL");
            ui->textEdit->setPlainText("");
        }
    });

    connect(ui->listWidget_2, SIGNAL(itemClicked(QListWidgetItem *)),
            ui->listWidget,
            SLOT(on_listWidget_2_itemClicked(QListWidgetItem *)));

    connect(ui->AddChatButton, &QPushButton::clicked, [this] {
        add_chat *chooseWindow = new add_chat(nullptr, this);
        chooseWindow->show();
    });

    connect(ui->memberButton, &QPushButton::clicked, [this] {
      add_member *memberWindow = new add_member(nullptr);
      memberWindow->show();
    });

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this] {
        update_chats();
        if (num_of_chats != size_of_answer) {
            update_chats_ui();
            num_of_chats = ui->listWidget_2->count();
            // todo
            ui->listWidget_2->setCurrentRow(current_chat);
        }
    });

    timer->start(2000);

    this->setFixedSize(1000, 600);
}

void MainWindow::update_chats() {
    json chat_ids;
    chat_names.clear();
    std::string response = endpoint.send_blocking(
        "get_chat_list " + std::to_string(user.get_user_id()));
    // todo check if some problems occurred
    assert(response != "FAIL");
    chat_ids = json::parse(response);

    for (auto x : chat_ids) {
        response =
            endpoint.send_blocking("get_chat_name " + x["chat_id"].dump());
        // todo check fail
        assert(response != "FAIL");
        chat_names.push_back(json::parse(response));
    }

    size_of_answer = chat_names.size();
}
void MainWindow::update_messages(std::size_t chat_id) {
    std::string response =
        endpoint.send_blocking("get_chat_history " + std::to_string(chat_id));
    // todo check fail
    assert(response != "FAIL");
    json messages = json::parse(response);

    for (auto mess : messages) {
        ui->textEdit->setWordWrapMode(
            QTextOption::WrapAtWordBoundaryOrAnywhere);
        QListWidgetItem *item = new QListWidgetItem;
        MessageViewIn *row = new MessageViewIn(
            QString::fromStdString(mess["message_text"].get<std::string>()));
        row->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        ui->listWidget->setSizeAdjustPolicy(QListWidget::AdjustToContents);
        ui->listWidget->setWordWrap(true);
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, row);
        item->setSizeHint(row->sizeHint());
        item->setFont(QFont("Helvetica [Cronyx]", 12));
    }
}

Ui::MainWindow *MainWindow::get_ui() const {
    return ui;
}

void MainWindow::on_listWidget_2_itemClicked(QListWidgetItem *item) {
    ui->listWidget->clear();
    current_chat = ui->listWidget_2->currentRow();
    update_messages(icon_to_chat_id[item]);
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
        icon_to_name[item]=QString::fromStdString(chat_name);
        ChatView *row = new ChatView(QString::fromStdString(chat_name));
        ui->listWidget_2->setWordWrap(true);
        ui->listWidget_2->addItem(item);
        ui->listWidget_2->setItemWidget(item, row);
        item->setSizeHint(row->minimumSizeHint());
        item->setFont(QFont("Helvetica [Cronyx]", 12));
        item->setSizeHint(QSize(2, 52));
    }
}
