#include "auth.h"
#include "ui_auth.h"
//#include <QText

auth::auth(QWidget *parent) : QDialog(parent), ui(new Ui::auth) {
    ui->setupUi(this);
    connect(ui->AuthButton, &QPushButton::clicked, [this] {
        if ((ui->lineEdit->text()).size() != 0) {
            ui->lineEdit->setText("");
            hide();
        }
    });
}

auth::~auth() {
    delete ui;
}
