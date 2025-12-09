#ifndef APP_PRESENTER_H
#define APP_PRESENTER_H

#include "interface_tcp_view.h"
#include "interface_presenter.h"
#include "interface_tcp_model.h"
#include <memory>
#include <iostream>
#include <QString>
#include <QMainWindow>
#include "formatted_message.h"
#include "message_handler.h"
#include "hand_shaker.h"

class AppPresenter : public IPresenter
{
    std::unique_ptr<ITcpClientModel> m_model{};
    std::unique_ptr<ITcpView> m_view{};
    std::unique_ptr<MessageHandler> m_message_handler{};
    HandShaker m_handshaker;
    // HandShakeMode m_handshake_mode{HandShakeMode::AwaitHandShake};
    // ConnectionResult m_connect_result{};
public:
    AppPresenter(std::unique_ptr<ITcpClientModel> model, std::unique_ptr<ITcpView> gui);

    // interface we expose to the gui:
    void set_port(uint16_t port) override;
    void send_message(const QString& msg) override;
    void disconnect() override;
    void set_up_connection(const QString& hostname) override;

    // interface we expose to the TCP client:
    void handle_connect_response(const ConnectionResult& connect_result) override;
    void handle_msg_reception(const MessageVariant& msg) override;
    void handle_disconnect_from_host() override;
    void handle_msg_send_failure(const MessageSendFailure& failed_msg) override;

    void show() override;
};



#endif // APP_PRESENTER_H
