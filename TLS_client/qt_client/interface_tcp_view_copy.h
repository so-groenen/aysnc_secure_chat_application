#ifndef INTERFACE_TCP_VIEW_COPY_H
#define INTERFACE_TCP_VIEW_COPY_H
#include "interface_tcp_client.h"
#include "interface_tcp_event_handler.h"
#include <QMainWindow>
#include <QString>
#include <interface_ssl_client.h>

// The TCP view (the GUI) reacts upon message reception, connection failures etc
// The "raw" low level TCP events are abstracted via the presenter which formats & perform error handling
// The View (ie, the Qt GUI) also needs to expose some methods so that the presenter is able to correctly format messages

class ITcpView : public ITcpEventHandler
{
public:
    virtual ~ITcpView() = default;
    virtual void attach(ISslClient* presenter) = 0;
    virtual const QString& get_password() const = 0;
    virtual const QString& get_username() const = 0;
    virtual QColor get_font_color() const = 0;
    virtual void set_default_hostname(QStringView host) = 0;
    virtual void set_password(QStringView password) = 0;
};


#endif // INTERFACE_TCP_VIEW_COPY_H
