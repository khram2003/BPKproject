#include "mainwindow.h"
#include <iostream>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QPixmap>
#include <QPushButton>
#include <QShortcut>
#include "chat_view.h"
#include "message_view.h"
#include "ui_mainwindow.h"
//#include <pqxx/pqxx>
//#include "../../server/server.cpp"
#include <unordered_map>
//#include <nlohmann/json.hpp>

//Саня сделай хедер ёлы палы а то че мы как бомжи



MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);


    std::string chat_list;
    std::size_t TEST_ID=1;
//    chat_list=database.get_chat_list(TEST_ID);
//    std::cout<<chat_list;

    //TODO:теперь нужно распарсить id и получить имя чата (вопрос Сане).
    std::vector<std::pair<QString, std::size_t>> chat_names_id = {
            {"Biba", 1}, {"Boba", 2}, {"Bobo", 3}};  // just for example, should be downloaded

    for (auto x : chat_names_id) {
        QListWidgetItem *item = new QListWidgetItem;
        ChatView *row = new ChatView(x.first);
        ui->listWidget_2->setWordWrap(true);
        ui->listWidget_2->addItem(item);
        ui->listWidget_2->setItemWidget(item, row);
        item->setSizeHint(row->minimumSizeHint());
        item->setFont(QFont("Helvetica [Cronyx]", 12));
        item->setSizeHint(QSize(2, 52));
        itemptr_to_chatid[item]=x.second;
    }

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
            ui->textEdit->setPlainText("");
//            database.add_message();
        }
    });

    connect(ui->listWidget_2, SIGNAL(itemClicked(QListWidgetItem *)),
            ui->listWidget,
            SLOT(on_listWidget_2_itemClicked(QListWidgetItem *)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_listWidget_2_itemClicked(QListWidgetItem *item) {
    ui->listWidget->clear();
//    std::string chat_history=database.get_chat_history(itemptr_to_chatid[item]);
}