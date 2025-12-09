#ifndef INTERFACE_TCP_CLIENT_H
#define INTERFACE_TCP_CLIENT_H

#include <QString>
#include <iostream>

// When an event occurs in Qt widget, we forward it to the presenter...
// the presenter needs the TCP client to expose the following methods:

class ITcpClient
{
public:
    virtual ~ITcpClient() = default;
    virtual void set_port(uint16_t port) = 0;
    virtual void set_up_connection(const QString& hostname) = 0;
    virtual void send_message(const QString& msg) = 0;
    virtual void disconnect() = 0;
};



#endif // INTERFACE_TCP_CLIENT_H
