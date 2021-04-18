#include "../include/message_view.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QString>
#include <QWidget>

[[maybe_unused]] MessageViewOut::MessageViewOut(const QString& msg) {
    QHBoxLayout *row = new QHBoxLayout();
    QLabel *lbl = new QLabel;
    lbl->setStyleSheet(
        "QLabel { background-color : #353535; color : white; border-width: "
        "2px; border-radius: 10px; padding: 6px;}");
    lbl->setAlignment(Qt::AlignCenter | Qt::AlignRight);
    lbl->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    lbl->setWordWrap(true);
    lbl->setText(msg);
    row->addWidget(lbl);

    setLayout(row);
}

MessageViewIn::MessageViewIn(const QString& msg) {
    QHBoxLayout *row = new QHBoxLayout();
    QLabel *lbl = new QLabel;
    QLabel *lblim = new QLabel;
    QPixmap npm("Sprites/qwertyuiop.jpg");
    QPixmap pm = npm.scaled(QSize(50, 50));
    lblim->setPixmap(pm);
    lblim->setScaledContents(true);
    lbl->setStyleSheet(
        "QLabel { background-color : #353535; color : white; border-width: "
        "2px; border-radius: 10px; padding: 6px;}");
    QSizePolicy spLeft(QSizePolicy::Minimum, QSizePolicy::Expanding);
    spLeft.setVerticalStretch(1);
    lbl->setSizePolicy(spLeft);
    lbl->setWordWrap(true);
    lbl->setText(msg);
    lbl->adjustSize();
    lblim->setStyleSheet("QLabel { border-radius: 10px}");
    lblim->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lblim->setAlignment(Qt::AlignBottom);
    row->addWidget(lblim);
    row->addWidget(lbl);
    setLayout(row);
}
