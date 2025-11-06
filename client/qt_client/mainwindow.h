#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    // getters & setters
signals:
    // this should be sent to Presenter
    void trying_to_connect(QString hostname);
    void disconnecting();
    void sending_message(QString msg);

public slots:
    // void on_connectBtn_clicked();
    void on_connectBtn_clicked();

    // ideally this would go to a "Presenter" between the "View"(mainWindow) and the "Model" (TcpClient)
    void handle_connect_response(bool value, const QString& Ipv4);
    void handle_msg_reception(const QString& msg);

    // should be handle error
    void handle_disconnect_from_host();
    void handle_message_sent_status(const QString& msg, bool is_sent);

private slots:
    // this should be private
    void on_SendBtn_clicked();

private:
    std::unique_ptr<Ui::MainWindow> ui{};
    // Ui::MainWindow* ui;

    // this should be in TCP client [tcpClient.socket.state == connected]
    bool m_is_connected{false};
    QString m_name{"me"};
    QString m_other{"<Server>"};
    void set_Btn_to_connect();
    void set_Btn_to_disconnect();

};
#endif // MAINWINDOW_H
