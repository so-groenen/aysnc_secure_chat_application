#ifndef INTERFACE_TCP_VIEW_H
#define INTERFACE_TCP_VIEW_H
#include "interface_tcp_client.h"
#include "interface_tcp_event_handler.h"
#include <QMainWindow>
#include <QString>


class ITcpView : public ITcpEventHandler
{
public:
    virtual ~ITcpView() = default;
    virtual void attach(ITcpClient* presenter) = 0;
    virtual const QString& get_password() const = 0;
    virtual const QString& get_username() const = 0;
    virtual QColor get_font_color() const = 0;
    virtual void set_default_hostname(QStringView host) = 0;
    virtual void set_password(QStringView password) = 0;

};


#endif // INTERFACE_TCP_VIEW_H
