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

private:
    Ui::view_boards *ui;
    PopUp *up;
    MainWindow *mess;
};

#endif  // VIEW_BOARDS_H
