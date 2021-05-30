#include <autowraplabel.h>
#include <message_view.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QSizePolicy>
#include <QString>
#include <QWidget>

[[maybe_unused]] MessageViewIn::MessageViewIn(const QString &msg,
                                              const QString &author) {
    QHBoxLayout *row_view = new QHBoxLayout();
    QVBoxLayout *row_message = new QVBoxLayout();
    row_message->setSizeConstraint(QLayout::SetMinimumSize);
    QLabel *lbl = new QLabel;
    QLabel *lbl_author = new QLabel;
    QLabel *lblim = new QLabel;
    QPixmap npm("Sprites/qwertyuiop.jpg");
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
    QSizePolicy spLeft(QSizePolicy::Fixed, QSizePolicy::Expanding);
    lbl->setSizePolicy(spLeft);
    lbl->setWordWrap(true);
    lbl->setText(msg);
    QFont f_m("Arial", 13);
    lbl->setFont(f_m);
    lbl->adjustSize();
    lblim->setStyleSheet("QLabel {background-color : #353535}");
    lblim->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lblim->setAlignment(Qt::AlignBottom);
    lbl_author->setText("Author: " + author);
    QFont f_a("Arial", 7);
    lbl_author->setFont(f_a);
    lbl_author->setStyleSheet("QLabel {padding: 4px; color: #a0a0a0}");
    row_message->addWidget(lbl);
    row_message->addWidget(lbl_author);
    row_view->addWidget(lblim);
    row_view->addLayout(row_message);

    setLayout(row_view);
}

MessageViewOut::MessageViewOut(const QString &msg) {
    QHBoxLayout *row = new QHBoxLayout();
    AutoWrapLabel *lbl = new AutoWrapLabel;
    QLabel *lblim = new QLabel;
    QPixmap npm("Sprites/avatar.jpg");
    QPixmap pm = npm.scaled(QSize(50, 50));

    lblim->setPixmap(pm);
    lblim->setFixedSize(50, 50);
    QRegion *region =
        new QRegion(0, 0, lblim->width(), lblim->height(), QRegion::Ellipse);
    lblim->setScaledContents(true);
    lblim->setMask(*region);
    lbl->setStyleSheet(
        "QLabel { background-color : #007ef4; color : white; border-width: "
        "2px; border-radius: 10px; padding: 6px;}");
    QSizePolicy spLeft(QSizePolicy::Fixed, QSizePolicy::Expanding);
    lbl->setSizePolicy(spLeft);
    //    lbl->setWordWrap(true);
    lbl->setText(msg);
    QFont f_m("Arial", 13);
    lbl->setFont(f_m);
    lbl->adjustSize();
    lblim->setStyleSheet("QLabel {background-color : #003366}");
    lblim->setAlignment(Qt::AlignBottom);
    row->addWidget(lbl, Qt::AlignRight);
    row->setAlignment(lbl, Qt::AlignRight);
    row->addWidget(lblim, Qt::AlignRight);
    row->setAlignment(lblim, Qt::AlignBottom);
    setLayout(row);
}
