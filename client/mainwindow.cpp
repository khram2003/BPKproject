#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QShortcut>
#include <QListWidgetItem>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QPixmap>

//хочется сделать отдельный класс для сообщения и что нибудь с путями к картинкам

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QPixmap sendButtonPixmap("C:/messenger/Sprites/sendButton.png");
    QPixmap avatarPixmap("");
    QIcon sendButtonIcon(sendButtonPixmap);
    ui->setupUi(this);
    ui->sendButton1->setIcon(sendButtonIcon);
    ui->sendButton2->setIcon(sendButtonIcon);
    ui->sendButton1->setIconSize(sendButtonPixmap.rect().size());
    ui->sendButton2->setIconSize(sendButtonPixmap.rect().size());

    connect(ui->sendButton1, &QPushButton::clicked, [this]{
        QListWidgetItem *item=new QListWidgetItem;
        QIcon *avatar=new QIcon("C:/messenger/Sprites/qwertyuiop.jpg");
        item->setText(ui->lineEdit->text());
        QColor messageBg(0,51,102);
        QBrush messageBrush(messageBg);
        item->setBackground(messageBrush);
        //item->setTextAlignment(Qt::AlignVCenter); //Хочется два параметра Alignment
        item->setTextAlignment(Qt::AlignLeft);
        item->setFont(QFont("Helvetica [Cronyx]", 12));
        item->setSizeHint(QSize(2, 52));
        item->setIcon(*avatar);
        //нет выравнивания icon((
        ui->listWidget->setIconSize(QSize(50, 50));
        ui->lineEdit->setText("");
        ui->listWidget->addItem(item);

    });

    connect(ui->sendButton2, &QPushButton::clicked, [this]{
        QListWidgetItem *item=new QListWidgetItem;
        QIcon *avatar=new QIcon("C:/Users/Никита/Desktop/trash/qwertyuiop.jpg");
        item->setText(ui->lineEdit->text());
        QColor messageBg(0,51,102);
        QBrush messageBrush(messageBg);
        item->setBackground(messageBrush);
        //item->setTextAlignment(Qt::AlignVCenter); //Хочется два параметра Alignment
        //временные меры
        item->setTextAlignment(Qt::AlignRight);
        item->setFont(QFont("Helvetica [Cronyx]", 12));
        item->setSizeHint(QSize(2, 52));
        item->setIcon(*avatar);
        //нет выравнивания icon((
        ui->listWidget->setIconSize(QSize(50, 50));
        ui->lineEdit->setText("");
        ui->listWidget->addItem(item);

    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

