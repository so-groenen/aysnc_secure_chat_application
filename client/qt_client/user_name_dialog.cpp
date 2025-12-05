#include "user_name_dialog.h"
#include "ui_user_name_dialog.h"

UsernameDialog::UsernameDialog(QString username, QColor color, bool clear_history_on_reconnect, QWidget *parent)
    : QDialog(parent), m_selected_color{color},
     ui(new Ui::UsernameDialog)
{
    ui->setupUi(this);
    ui->userEdit->setText(username);
    ui->HistoryCheckBox->setChecked(clear_history_on_reconnect);
}

UsernameDialog::~UsernameDialog()
{
    delete ui;
}

QString UsernameDialog::get_username() const
{
    return ui->userEdit->text();
}

const QColor &UsernameDialog::selected_color() const
{
    return m_selected_color;
}

bool UsernameDialog::should_clear_history_on_reconnect() const
{
    return ui->HistoryCheckBox->isChecked();
}

void UsernameDialog::on_pushButton_clicked()
{
    QColorDialog color_dialog{};
    color_dialog.setCurrentColor(m_selected_color);
    color_dialog.setModal(true);
    if(color_dialog.exec() ==  QDialog::Accepted)
    {
        m_selected_color = color_dialog.selectedColor();
        qDebug() << "QColorDialog: selected: " << m_selected_color;
    }
}

