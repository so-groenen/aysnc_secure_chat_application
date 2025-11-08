#ifndef INTERFACE_TCP_VIEW_H
#define INTERFACE_TCP_VIEW_H
#include "interface_tcp_client.h"
#include "interface_tcp_event_handler.h"
#include <QMainWindow>


class ITcpView : public ITcpEventHandler
{
public:
    virtual ~ITcpView() = default;
    virtual void attach(ITcpClient* presenter) = 0;
};


#endif // INTERFACE_TCP_VIEW_H
