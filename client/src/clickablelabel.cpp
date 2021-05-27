#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent,
                               [[maybe_unused]] Qt::WindowFlags f,
                               const QString &trello_state)
    : QLabel(parent) {
    setText(trello_state);
    setStyleSheet("font: 7pt;");

}

ClickableLabel::~ClickableLabel() {
}

void ClickableLabel::mousePressEvent([[maybe_unused]] QMouseEvent *event) {
    emit clicked();
}
