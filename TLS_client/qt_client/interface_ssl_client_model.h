#ifndef INTERFACE_TCP_MODEL_H
#define INTERFACE_TCP_MODEL_H

#include "interface_tcp_client.h"
#include "interface_tcp_event_handler.h"


// The TcpClientModel is a TCP Client which can attach to an Event Handler (the presenter) that will be called to, well, handle events

class ITcpClientModel : public ITcpClient
{
public:
    virtual ~ITcpClientModel() = default;
    virtual void attach(ITcpEventHandler* presenter) = 0;
    virtual const QString& get_delimiter() const = 0;
};


#endif // INTERFACE_TCP_MODEL_H
