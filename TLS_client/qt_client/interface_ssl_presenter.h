#ifndef INTERFACE_SSL_PRESENTER_H
#define INTERFACE_SSL_PRESENTER_H

#include "interface_tcp_event_handler.h"
#include "interface_ssl_client.h"

class ISslPresenter : public ISslClient, public ITcpEventHandler
{
public:
    virtual ~ISslPresenter() = default;
    virtual void show() = 0;
};

#endif // INTERFACE_SSL_PRESENTER_H
