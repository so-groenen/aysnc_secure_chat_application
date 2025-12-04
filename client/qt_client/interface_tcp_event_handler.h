#ifndef INTERFACE_TCP_EVENT_HANDLER_H
#define INTERFACE_TCP_EVENT_HANDLER_H
#include <QString>
#include "message_variant.h"

// When an event occurs in Tcp Client, we forward it to the presenter..
// the presenter needs the GUI (which need to handle the TCP Client events) to expose the following methods:

class ITcpEventHandler
{
public:
    virtual ~ITcpEventHandler() = default;
    virtual void handle_connect_response(bool is_connected, const QString& ipv4) = 0;
    //virtual void handle_msg_reception(const QString& msg) = 0;
    virtual void handle_msg_reception(const MessageVariant& msg) = 0;
    virtual void handle_disconnect_from_host() = 0;
    virtual void handle_msg_sent_status(QString msg, bool is_sent) = 0;
};



#endif // INTERFACE_TCP_EVENT_HANDLER_H
