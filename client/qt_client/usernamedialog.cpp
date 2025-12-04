#include "usernamedialog.h"
#include "ui_usernamedialog.h"

UsernameDialog::UsernameDialog(QString username, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UsernameDialog)
{
    ui->setupUi(this);
    ui->userEdit->setText(username);
}

UsernameDialog::~UsernameDialog()
{
    delete ui;
}

QString UsernameDialog::get_username() const
{
    return ui->userEdit->text();
}
