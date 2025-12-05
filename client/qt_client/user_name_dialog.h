#ifndef USER_NAME_DIALOG_H
#define USER_NAME_DIALOG_H

#include <QDialog>
#include <QString>
#include <QColorDialog>

namespace Ui {
class UsernameDialog;
}

class UsernameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UsernameDialog(QString username, QColor color, bool clear_history_on_reconnect, QWidget *parent = nullptr);
    ~UsernameDialog();
    QString get_username() const;
    const QColor& selected_color() const;
    bool should_clear_history_on_reconnect() const;
private slots:
    void on_pushButton_clicked();

private:
    Ui::UsernameDialog *ui;
    QColor m_selected_color{};
};

#endif // USER_NAME_DIALOG_H

