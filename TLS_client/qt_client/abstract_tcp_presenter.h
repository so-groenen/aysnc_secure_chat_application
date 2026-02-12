#ifndef ABSTRACT_TCP_PRESENTER_H
#define ABSTRACT_TCP_PRESENTER_H

#include "interface_tcp_view.h"
#include <QString>
#include <QMainWindow>

class AbstractTcpPresenter : public ITcpClient, public ITcpEventHandler
{
protected:
    ITcpView* m_view{};
public:
    virtual ~AbstractTcpPresenter() = default;
    virtual void show() = 0;
    virtual void attach(ITcpView* tcp_view) = 0;
    virtual void should_broadcast_name(bool val) = 0;
};
using AbstractTcpPresenter_ptr = std::unique_ptr<AbstractTcpPresenter>;


#endif // ABSTRACT_TCP_PRESENTER_H
