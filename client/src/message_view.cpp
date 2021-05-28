#include <message_view.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QSizePolicy>
#include <QString>
#include <QWidget>

[[maybe_unused]] MessageViewIn::MessageViewIn(const QString &msg) {
    QHBoxLayout *row = new QHBoxLayout();
    QLabel *lbl = new QLabel;
    QLabel *lblim = new QLabel;
    QPixmap npm("Sprites/qwerеyuiop.jpg");
    QPixmap pm = npm.scaled(QSize(50, 50));
    lblim->setPixmap(pm);
    lblim->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lblim->setFixedSize(50, 50);
    QRegion *region =
        new QRegion(0, 0, lblim->width(), lblim->height(), QRegion::Ellipse);
    lblim->setScaledContents(true);
    lblim->setMask(*region);
    lbl->setStyleSheet(
        "QLabel { background-color : #353535; color : white; border-width: "
        "2px; border-radius: 10px; padding: 6px;}");
    QSizePolicy spLeft(QSizePolicy::Minimum, QSizePolicy::Expanding);
    spLeft.setVerticalStretch(1);
    lbl->setSizePolicy(spLeft);
    lbl->setWordWrap(true);
    lbl->setText(msg);
    lbl->adjustSize();
    lblim->setStyleSheet(
        "QLabel { border-radius: 10px, background-color : #353535}");
    lblim->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lblim->setAlignment(Qt::AlignBottom);
    row->addWidget(lblim);
    row->addWidget(lbl);
    setLayout(row);
}

MessageViewOut::MessageViewOut(const QString &msg) {
    QHBoxLayout *row = new QHBoxLayout();
    QLabel *lbl = new QLabel;
    QLabel *lblim = new QLabel;
    QPixmap npm("Sprites/avatar.jpg");
    QPixmap pm = npm.scaled(QSize(50, 50));

    lblim->setPixmap(pm);
    lblim->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lblim->setFixedSize(50, 50);
    QRegion *region =
        new QRegion(0, 0, lblim->width(), lblim->height(), QRegion::Ellipse);
    lblim->setScaledContents(true);
    lblim->setMask(*region);
    lbl->setStyleSheet(
        "QLabel { background-color : #007ef4; color : white; border-width: "
        "2px; border-radius: 10px; padding: 6px;}");
    QSizePolicy spLeft(QSizePolicy::Minimum, QSizePolicy::Expanding);
    spLeft.setVerticalStretch(1);
    lbl->setSizePolicy(spLeft);
    lbl->setWordWrap(true);
    lbl->setText(msg);
    lbl->adjustSize();
    lbl->setAlignment(Qt::AlignRight);
    lblim->setStyleSheet(
        "QLabel { border-radius: 10px, background-color : #003366}");
    lblim->setAlignment(Qt::AlignBottom);
    row->addWidget(lbl);
    row->addWidget(lblim);
    setLayout(row);
}
