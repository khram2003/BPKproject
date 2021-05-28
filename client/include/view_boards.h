#ifndef VIEW_BOARDS_H
#define VIEW_BOARDS_H

#include <mainwindow.h>
#include <popup.h>
#include <ui_view_boards.h>
#include <QMainWindow>

namespace Ui {
class view_boards;
}

class view_boards : public QMainWindow {
    Q_OBJECT

public:
    explicit view_boards(QWidget *parent = nullptr,
                         MainWindow *messWin = nullptr);

    ~view_boards() override;

    std::unordered_map<QListWidgetItem *, std::string> icon_to_board_id;

private slots:

    void on_listWidget_itemClicked(QListWidgetItem *item);


private:
    Ui::view_boards *ui;
    PopUp *up;
    MainWindow *mess;
    std::string chatter_token;
    std::string chatter_id;
};

#endif  // VIEW_BOARDS_H
