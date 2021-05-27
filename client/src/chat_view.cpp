#include <chat_view.h>
#include <trello.h>
#include <clickablelabel.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QString>
#include <QWidget>

ChatView::ChatView(const QString &name) {
    auto *row = new QVBoxLayout();
    auto *lbl = new QLabel;

    auto *clcklabel= new ClickableLabel(Q_NULLPTR, Qt::WindowFlags(), name);
    lbl->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    lbl->setWordWrap(true);
    lbl->setText(name);
    row->addWidget(lbl);
    row->addWidget(clcklabel);
    setLayout(row);
}

