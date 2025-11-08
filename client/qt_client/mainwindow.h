#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "interface_presenter.h"
#include "interface_tcp_view.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE



class MainWindow : public QMainWindow, public ITcpView
{
    Q_OBJECT

private:
    std::unique_ptr<Ui::MainWindow> ui{};
    ITcpClient* m_presenter{};
    bool m_is_connected{false};
    QString m_name{"me"};
    QString m_other{"<Server>"};
    void set_Btn_to_connect();
    void set_Btn_to_disconnect();

public:
    MainWindow(QWidget *parent = nullptr);
    void attach(ITcpClient* presenter) override;
    void handle_connect_response(bool is_connected, const QString& Ipv4) override;
    void handle_msg_reception(const QString& msg) override;
    void handle_disconnect_from_host() override;
    void handle_msg_sent_status(QString msg, bool is_sent) override;

private slots:
    // this should be private
    void on_SendBtn_clicked();
    void on_connectBtn_clicked();


};
#endif // MAINWINDOW_H
