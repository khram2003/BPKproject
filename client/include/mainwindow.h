#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>
#include <QStringListModel>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void update_chats();

    void update_messages(std::size_t chat_id);

    void update_chats_ui();

    Ui::MainWindow *get_ui() const;

    std::unordered_map<QListWidgetItem *, std::size_t> icon_to_chat_id;
    std::unordered_map<QListWidgetItem *, QString> icon_to_name;

signals:

    void authWin();

private slots:

    void on_listWidget_2_itemClicked(QListWidgetItem *item);

signals:

    void messWin();

private:
    Ui::MainWindow *ui;
    int num_of_chats = 0;
    int size_of_answer = 0;
    json chat_names;
    int current_chat = 0;
};

#endif  // MAINWINDOW_H
