#ifndef USER_NAME_DIALOG_H
#define USER_NAME_DIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class UsernameDialog;
}

class UsernameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UsernameDialog(QString username, QWidget *parent = nullptr);
    ~UsernameDialog();
    QString get_username() const;

private:
    Ui::UsernameDialog *ui;
};

#endif // USER_NAME_DIALOG_H
