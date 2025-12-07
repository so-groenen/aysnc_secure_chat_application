#include "user_name_dialog.h"
#include "ui_user_name_dialog.h"

UsernameDialog::UsernameDialog(QString username, QColor color, DelegateMode delegate_mode, QWidget *parent)
    : QDialog(parent),
    m_managed_ui{std::make_unique<Ui::UsernameDialog>()},
    m_selected_color{color}, m_delegate_mode{delegate_mode},
    ui(m_managed_ui.get())
{
    ui->setupUi(this);
    ui->userEdit->setText(username);

    switch (delegate_mode)
    {
        case DelegateMode::BubbleDelegate:
            ui->bubbleRadio->toggle();
            break;
        case DelegateMode::LineMessageDelegate:
            ui->lineRadio->toggle();
            break;
        default:
            break;
    }
}

UsernameDialog::~UsernameDialog()
{
}

QString UsernameDialog::get_username() const
{
    return ui->userEdit->text();
}

const QColor &UsernameDialog::selected_color() const
{
    return m_selected_color;
}

DelegateMode UsernameDialog::get_delegate_mode() const
{
    return m_delegate_mode;
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


void UsernameDialog::on_bubbleRadio_toggled(bool checked)
{
    if(checked)
        m_delegate_mode = DelegateMode::BubbleDelegate;
}


void UsernameDialog::on_lineRadio_toggled(bool checked)
{
    if(checked)
        m_delegate_mode = DelegateMode::LineMessageDelegate;
}

