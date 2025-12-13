#ifndef ABSTRACT_TCP_MODEL_H
#define ABSTRACT_TCP_MODEL_H

#include <QString>
#include "interface_tcp_client.h"
#include "interface_tcp_event_handler.h"

// When an event occurs in Qt widget, we forward it to the presenter...
// the presenter needs the TCP client to expose the following methods:



class AbstractTcpClientModel : public ITcpClient
{
// protected:
//     ITcpEventHandler* m_tcp_event_handler{};
public:
    // void attach(ITcpEventHandler* handler)
    // {
    //     m_tcp_event_handler = handler;
    // }
    virtual void attach(ITcpEventHandler* presenter) = 0;
    virtual const QString& get_delimiter() const = 0;

};
using AbstractTcpModel_ptr = std::unique_ptr<AbstractTcpClientModel>;



#endif // ABSTRACT_TCP_MODEL_H
