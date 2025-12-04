#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "interface_presenter.h"
#include "interface_tcp_view.h"
#include <QTextEdit>

#include <iostream>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE



class MainWindow : public QMainWindow, public ITcpView
{
    Q_OBJECT

private:
    Ui::MainWindow* ui;
    ITcpClient* m_presenter{};
    bool m_is_connected{false};
    QString m_username{"Your name"};
    QString m_name{"me"};
    QString m_password{"louvre"};           // should get those from presenter
    uint16_t m_port{6970};                  // should get those from tcp server via presenter
    QString m_hostname{""};                 // should get those from tcp server via presenter
    void set_Btn_to_connect();
    void set_Btn_to_disconnect();
    void set_username();

public:
    ~MainWindow();
    MainWindow(QWidget *parent = nullptr);
    void set_default_hostname(QStringView host);
    void set_password(QStringView password);

    // TCP Event Handler
    void handle_connect_response(bool is_connected, const QString& Ipv4) override;
    void handle_msg_reception(const MessageVariant &msg) override;
    void handle_disconnect_from_host() override;
    void handle_msg_sent_status(QString msg, bool is_sent) override;

    // TCP View
    void attach(ITcpClient* presenter) override;
    const QString& get_password() const override;
    const QString& get_username() const override;
private slots:
    void on_SendBtn_clicked();
    void on_connectBtn_clicked();
    void on_actionEdit_Server_triggered();
    void on_actionEdit_Username_triggered();
};
#endif // MAINWINDOW_H
