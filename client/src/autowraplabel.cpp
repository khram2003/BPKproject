#include <autowraplabel.h>
#include <QFontMetrics>

AutoWrapLabel::AutoWrapLabel(QWidget *parent) : QLabel(parent) {
}

AutoWrapLabel::AutoWrapLabel(const QString &text,
                             QWidget *parent,
                             Qt::WindowFlags f)
    : QLabel(text, parent, f) {
    setText(text);
}

AutoWrapLabel::~AutoWrapLabel() {
}

void AutoWrapLabel::setText(const QString &qsText) {
    m_qsText = qsText;
    InitText();
}

void AutoWrapLabel::setTitleText(const QString &qsText) {
    m_qsText = qsText;
    InitText();
}

void AutoWrapLabel::resizeEvent(QResizeEvent *event) {
    QLabel::resizeEvent(event);
    InitText();
}

void AutoWrapLabel::InitText() {
    QLabel::setText(CalcText(m_qsText));
}

QString AutoWrapLabel::CalcText(const QString &qsText) {
    QString text;
    QString tempText;
    int nHight = this->size().height();
    int nWidth = this->size().width();
    QFontMetrics metrics = this->fontMetrics();
    int nFontHight = metrics.height();
    int nMargin = this->margin();
    // a few lines
    int hightNum = (nHight - 2 * nMargin) / nFontHight;
    for (int index = 0; index < qsText.size() && hightNum > 0; ++index) {
        if (metrics.horizontalAdvance(tempText + qsText.at(index)) > nWidth) {
            text += tempText;
            --hightNum;
            if (hightNum > 0) {
                text += "\n";
            }
            tempText = qsText.at(index);
        } else {
            tempText += qsText.at(index);
        }
    }
    if (hightNum > 0) {
        text += tempText;
    }
    return text;
}
