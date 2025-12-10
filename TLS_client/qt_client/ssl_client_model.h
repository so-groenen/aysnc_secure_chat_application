#ifndef SSL_CLIENT_MODEL_H
#define SSL_CLIENT_MODEL_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QHostInfo>
#include <QHostAddress>
#include <QSslConfiguration>

#include <QIODevice>
#include "interface_tcp_event_handler.h"
#include "interface_ssl_client_model.h"
#include <QSslSocket>

class SslClientModel : public QObject, public ISslClientModel
{
    Q_OBJECT
private:
    ITcpEventHandler* m_presenter{};
    QSslSocket m_ssl_socket{};
    QTcpSocket m_socket{};
    QSslConfiguration m_config;
    uint16_t m_port{};
    QString m_delim{"\r\n"};
    QString m_hostname{};
    QHostAddress m_server_address{};
    bool m_is_connected{false};
public:
    explicit SslClientModel(uint16_t port, QObject *parent = nullptr);

    bool is_connected() const;

    //Secure Socket Layer
    void set_private_key(const QSslKey& private_key) override;
    void set_public_key(const QSslCertificate& public_key) override;
    void set_root_CA(const QSslCertificate& rootCA) override ;

    //TCP interface
    void set_port(uint16_t port) override;
    void send_message(const QString& msg) override;
    void disconnect() override;
    void set_up_connection(const QString& hostname) override;
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

#endif // SSL_CLIENT_MODEL_H
