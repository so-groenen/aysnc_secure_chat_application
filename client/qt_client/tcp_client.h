#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QHostInfo>
#include <QHostAddress>
#include <QIODevice>
#include "interface_tcp_event_handler.h"
#include "interface_tcp_model.h"

class TcpClient : public QObject, public ITcpClientModel
{
    Q_OBJECT
private:
    ITcpEventHandler* m_presenter{};
    QTcpSocket m_socket{};
    uint16_t m_port{};
    QString m_delim{"\r\n"};
    QString m_hostname{};
    QHostAddress m_server_address{};
    bool m_is_connected{false};

public:
    explicit TcpClient(uint16_t port, QObject *parent = nullptr);

    bool is_connected() const;

    //TCP interface
    void set_port(uint16_t port) override;
    void send_message(QString msg) override;
    void disconnect() override;
    void set_up_connection(QString hostname) override;
    void attach(ITcpEventHandler* presenter) override;
    const QString& get_delimiter() const override;

// connected internally to socket signals
private slots:
    void look_up_host_infos(const QHostInfo &host);
    void connecting_to_host();
    void recieving_msg();
    void is_disconnected();
    void retrieve_error(QAbstractSocket::SocketError socket_error);


};

#endif // TCP_CLIENT_H
