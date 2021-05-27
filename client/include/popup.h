#ifndef POPUP_H
#define POPUP_H

#include <QGridLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>

class PopUp : public QWidget {
    Q_OBJECT

    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity)

    void setPopupOpacity(float opacity);
    float getPopupOpacity() const;

public:
    explicit PopUp(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);

public slots:
    void setPopupText(const QString &text);
    void show();

private slots:
    void hideAnimation();
    void hide();

private:
    QLabel label;
    QGridLayout layout;
    QPropertyAnimation animation;
    float popupOpacity;
    QTimer *timer;
};

#endif  // POPUP_H
