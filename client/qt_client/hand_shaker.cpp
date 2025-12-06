#include "hand_shaker.h"

HandShaker::HandShaker(ITcpClientModel *tcp_model, ITcpView *tcp_view)
    : m_model{tcp_model}, m_view{tcp_view}
{

}

HandShaker::Result HandShaker::parse_session_id(const QString &message)
{
    qDebug() << "AWAITING SESSION ID:" << message;
    bool ok      = false;
    m_session_id = message.toLongLong(&ok);

    if(!ok)
    {
        qDebug() << "Connection rejected";
        ConnectionResult result = std::unexpected{ConnectionError{"SERVER REJECTED CONNECTION: WRONG PASSWORD"}};
        m_view->handle_connect_response(result);
        return HandShaker::Result::Fail;
    }
    m_handshake_mode  = HandShakeMode::Ok;
    m_view->handle_connect_response(m_connection_result);
    return HandShaker::Result::Success;
}

void HandShaker::await_handshake(const ConnectionResult& connection_result)
{
    m_connection_result = connection_result;
    auto password = m_view->get_password();
    m_model->send_message(password);
    m_handshake_mode = HandShakeMode::AwaitSessionId;
}

auto HandShaker::make_message_handler() const -> std::unique_ptr<MessageHandler>
{
    return std::make_unique<MessageHandler>(m_view->get_username(), m_view->get_font_color(), m_session_id);
}

void HandShaker::reset()
{
    m_handshake_mode    = HandShakeMode::AwaitHandShake;
    m_session_id        = -1;
    m_connection_result = ConnectionResult{};
}

bool HandShaker::is_awaiting_session_id() const
{
    return (m_handshake_mode == HandShakeMode::AwaitSessionId);
}

bool HandShaker::has_session_id() const
{
    return  (m_handshake_mode == HandShakeMode::Ok);
}


