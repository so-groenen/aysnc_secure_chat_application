#ifndef APP_PRESENTER_H
#define APP_PRESENTER_H

#include "interface_tcp_view.h"
#include "interface_ssl_presenter.h"
#include "interface_ssl_client_model.h"
#include <memory>
#include <iostream>
#include <QString>
#include <QMainWindow>
#include "formatted_message.h"
#include "message_handler.h"
#include "hand_shaker.h"

class AppPresenter : public ISslPresenter
{
    std::unique_ptr<ISslClientModel> m_model{};
    std::unique_ptr<ISslView> m_view{};
    std::unique_ptr<MessageHandler> m_message_handler{};
    HandShaker m_handshaker;
    // HandShakeMode m_handshake_mode{HandShakeMode::AwaitHandShake};
    // ConnectionResult m_connect_result{};
public:
    AppPresenter(std::unique_ptr<ISslClientModel> model, std::unique_ptr<ISslView> gui);

    // interface we expose to the gui:
    void set_port(uint16_t port) override;
    void send_message(const QString& msg) override;
    void disconnect() override;
    void set_up_connection(const QString& hostname) override;

    // Expose SSL methods to the GUI:
    void set_private_key(const QSslKey& private_key) override;
    void set_public_key(const QSslCertificate& public_key) override;
    void set_root_CA(const QSslCertificate& rootCA) override;

    // interface we expose to the TCP client:
    void handle_connect_response(const ConnectionResult& connect_result) override;
    void handle_msg_reception(const MessageVariant& msg) override;
    void handle_disconnect_from_host() override;
    void handle_msg_send_failure(const MessageSendFailure& failed_msg) override;

    void show() override;
};



#endif // APP_PRESENTER_H
