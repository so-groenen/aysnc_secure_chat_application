#ifndef INTERFACE_PRESENTER_H
#define INTERFACE_PRESENTER_H

#include "interface_tcp_client.h"
#include "interface_tcp_event_handler.h"

class IPresenter : public ITcpClient, public ITcpEventHandler
{
public:
    virtual ~IPresenter() = default;
    virtual void show() = 0;
};

#endif // INTERFACE_PRESENTER_H
