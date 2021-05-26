#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <popup.h>
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

    std::size_t get_current_chat_id();

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
    std::size_t num_of_chats = 0;
    std::size_t size_of_answer = 0;
    json chat_names;
    std::size_t current_chat = 0;
    std::size_t current_chat_id;
    PopUp *up;
};

#endif  // MAINWINDOW_H
