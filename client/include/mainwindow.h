#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>
#include <QStringListModel>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void update_chats();

    Ui::MainWindow *get_ui() const;

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
