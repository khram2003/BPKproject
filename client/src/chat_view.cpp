#include <chat_view.h>
#include <trello.h>
#include <clickablelabel.h>
#include <QVBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QString>
#include <QWidget>

ChatView::ChatView(const QString &name) {
    row = new QVBoxLayout();
    lbl = new QLabel;

    lbl->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    lbl->setWordWrap(true);
    lbl->setText(name);
    row->addWidget(lbl);
    setLayout(row);
}

