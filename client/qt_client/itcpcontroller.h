#ifndef ITCPCONTROLLER_H
#define ITCPCONTROLLER_H

#include <QObject>

class ITcpController : public QObject
{
    Q_OBJECT
public:
    explicit ITcpController(QObject *parent = nullptr);

    //We ask the TCP client, which we have acces to, to set up a connection
    virtual void set_up_connection(QString hostname) = 0;

    //We can ask the TCP client to disconnect?
    virtual void disconnect() = 0;

public slots:
    //The TCP client handles message reception and then passes it to the controller
    virtual void handle_msg_reception(const QString& msg) = 0;

    //Here we get Host-connect response from the TCP client
    virtual void handle_connect_response(bool value, const QString& Ipv4) = 0;

    //  --> hen sending messages we need to be aware of the status
    virtual void handle_message_sent_status(const QString& msg, bool is_sent) = 0;

    // --> should be handle error, with err msg
    virtual void handle_disconnect_from_host() = 0;

signals:
};

#endif // ITCPCONTROLLER_H
