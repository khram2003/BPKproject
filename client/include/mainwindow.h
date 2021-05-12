#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>
#include <QStringListModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    std::unordered_map<QListWidgetItem*, std::size_t> itemptr_to_chatid;
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_listWidget_2_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
};

#endif  // MAINWINDOW_H
