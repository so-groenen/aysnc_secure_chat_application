#ifndef INTERFACE_SSL_CLIENT_MODEL_H
#define INTERFACE_SSL_CLIENT_MODEL_H

#include "interface_tcp_client.h"
#include "interface_tcp_event_handler.h"
#include "interface_ssl_client.h"

// The TcpClientModel is a TCP Client which can attach to an Event Handler (the presenter) that will be called to, well, handle events

class ISslClientModel : public ISslClient
{
public:
    virtual ~ISslClientModel() = default;
    virtual void attach(ITcpEventHandler* presenter) = 0;
    virtual const QString& get_delimiter() const = 0;
};


#endif // INTERFACE_SSL_CLIENT_MODEL_H
