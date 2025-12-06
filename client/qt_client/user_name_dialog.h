#ifndef USER_NAME_DIALOG_H
#define USER_NAME_DIALOG_H

#include <QDialog>
#include <QString>
#include <QColorDialog>
#include "message_delegate_modes.h"

namespace Ui {
class UsernameDialog;
}

class UsernameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UsernameDialog(QString username, QColor color, DelegateMode delegate_mode, QWidget *parent = nullptr);
    ~UsernameDialog();
    QString get_username() const;
    const QColor& selected_color() const;
    DelegateMode get_delegate_mode() const;

private slots:
    void on_pushButton_clicked();
    void on_bubbleRadio_toggled(bool checked);

    void on_lineRadio_toggled(bool checked);

private:
    Ui::UsernameDialog *ui;
    QColor m_selected_color{};
    DelegateMode m_delegate_mode{};
};

#endif // USER_NAME_DIALOG_H

