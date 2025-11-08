#ifndef APP_PRESENTER_H
#define APP_PRESENTER_H

#include "interface_tcp_view.h"
#include "interface_presenter.h"
#include "interface_tcp_model.h"
#include <memory>
#include <iostream>
#include <QString>
#include <QMainWindow>

class AppPresenter : public IPresenter
{
    std::unique_ptr<ITcpClientModel> m_model{};
    std::unique_ptr<ITcpView> m_view{};
public:
    AppPresenter(std::unique_ptr<ITcpClientModel> model, std::unique_ptr<ITcpView> gui);

    // interface we expose to the gui:
    void set_port(uint16_t port) override;
    void send_message(QString msg) override;
    void disconnect() override;
    void set_up_connection(QString hostname) override;

    // interface we expose to the TCP client:
    void handle_connect_response(bool value, const QString& Ipv4) override;
    void handle_msg_reception(const QString& msg) override;
    void handle_disconnect_from_host() override;
    void handle_msg_sent_status(QString msg, bool is_sent) override;

    // this function is very hacky, since we down cast ITcpView to a MainWindow ...
    void show() override;
};



#endif // APP_PRESENTER_H
