#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QWidget>
#include <QStringListModel>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QWidget {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    void authWin();

private slots:
    void on_listWidget_2_itemClicked(QListWidgetItem *item);

signals:
    void messWin();

private:
    Ui::MainWindow *ui;
};

#endif  // MAINWINDOW_H

