#include "../include/chat_view.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QString>
#include <QWidget>

ChatView::ChatView(const QString& name) {
    auto *row = new QHBoxLayout();
    auto *lbl = new QLabel;
    lbl->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    lbl->setWordWrap(true);
    lbl->setText(name);
    row->addWidget(lbl);
    setLayout(row);
}
