#ifndef INTERFACE_TCP_MODEL_H
#define INTERFACE_TCP_MODEL_H

#include "interface_tcp_client.h"
#include "interface_tcp_event_handler.h"


class ITcpClientModel : public ITcpClient
{
public:
    virtual ~ITcpClientModel() = default;
    virtual void attach(ITcpEventHandler* presenter) = 0;
};


#endif // INTERFACE_TCP_MODEL_H
