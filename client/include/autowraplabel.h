
#ifndef AUTOWRAPLABEL_H
#define AUTOWRAPLABEL_H
#include <QLabel>
#include <QResizeEvent>

class AutoWrapLabel : public QLabel {
    Q_OBJECT
public:
    AutoWrapLabel(QWidget *parent = nullptr);
    AutoWrapLabel(const QString &text,
                  QWidget *parent = nullptr,
                  Qt::WindowFlags f = Qt::WindowFlags());
    ~AutoWrapLabel();

    void setText(const QString &qsText);
    void setTitleText(const QString &qsText);
    void resizeEvent(QResizeEvent *event) override;

private:
    void InitText();
    QString CalcText(const QString &qsText);

private:
    QString m_qsText;
};

#endif