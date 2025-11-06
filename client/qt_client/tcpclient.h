#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QHostInfo>
#include <QHostAddress>
#include <QIODevice>

class TcpClient : public QObject
{
    Q_OBJECT
private:
    QTcpSocket m_socket{};
    uint32_t m_port{};
    QString m_hostname{};
    QHostAddress m_server_address{};
    bool m_is_connected{false};

public:
    explicit TcpClient(uint32_t Port = 1234, QObject *parent = nullptr);
    bool is_connected() const;
// To be sent to GUI
// can be sent to a Presenter?
signals:
    void ready_to_connect();
    void connection_result(bool value, const QString& Ipv4);
    void recieved_msg(const QString& msg);
    void message_sent_status(QString msg, bool is_sent);
    void is_disconneced_from_Host();


// connected to UI signals
// can be transfered to Presenter?
// should be public methods accessible to Presenter / TCPcontroller
public slots:
    void set_up_connection(QString hostname);
    void disconnect();
    void send_message(QString msg);

// Connected to socket signals
private slots:
    void look_up_host_infos(const QHostInfo &host);
    void connecting_to_host();
    void recieving_msg();
    void is_disconnected();

    // should replace is_disconnected_from_Host
    void retrieve_error(QAbstractSocket::SocketError socketError);


};

#endif // TCPCLIENT_H
