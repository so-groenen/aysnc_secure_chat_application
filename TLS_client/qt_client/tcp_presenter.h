#ifndef TCP_PRESENTER_H
#define TCP_PRESENTER_H

#include "abstract_tcp_presenter.h"
#include "abstract_tcp_model.h"
#include <memory>
#include <QString>
#include <QMainWindow>
// #include "formatted_message.h"
#include "message_handler.h"
#include "hand_shaker.h"



class TcpPresenter : public AbstractTcpPresenter
{
protected:
    AbstractTcpModel_ptr m_model{};
private:
    std::unique_ptr<MessageHandler> m_message_handler{};
    std::unique_ptr<HandShaker> m_handshaker{};
    bool m_should_broadcast_name{};
    // HandShakeMode m_handshake_mode{HandShakeMode::AwaitHandShake};
    // ConnectionResult m_connect_result{};
public:
    TcpPresenter(AbstractTcpModel_ptr model);

    // interface we expose to the gui:
    void set_port(uint16_t port) override;
    void send_message(const QString& msg) override;
    void disconnect() override;
    void set_up_connection(const QString& hostname) override;
    void attach(ITcpView* tcp_view) override;
    void should_broadcast_name(bool val) override;


    // interface we expose to the TCP client:
    void handle_connect_response(const ConnectionResult& connect_result) override;
    void handle_msg_reception(const MessageVariant& msg) override;
    void handle_disconnect_from_host() override;
    void handle_msg_send_failure(const MessageSendFailure& failed_msg) override;

    void show() override;
};

using TcpPresenter_ptr = std::unique_ptr<TcpPresenter>;



#endif // TCP_PRESENTER_H
