#ifndef HAND_SHAKER_H
#define HAND_SHAKER_H
#include "interface_tcp_view.h"
#include "interface_tcp_client.h"
#include <QString>
#include <memory>
#include "message_handler.h"


class HandShaker
{
public:
    enum class Result
    {
        Success,
        Fail,
    };
private:
    enum class HandShakeMode
    {
        AwaitHandShake,
        AwaitSessionId,
        Ok
    };

    ITcpClient* m_model{};
    ITcpView* m_view{};
    qint64 m_session_id{-1};
    HandShakeMode m_handshake_mode{HandShakeMode::AwaitHandShake};
    ConnectionResult m_connection_result{};
public:
    explicit HandShaker(ITcpClient* tcp_client, ITcpView* tcp_view);
    // explicit HandShaker(ISslClientModel* tcp_model, ITcpView* tcp_view);
    Result parse_session_id(const QString &message);
    void await_handshake(const ConnectionResult& connection_result);
    auto make_message_handler() const -> std::unique_ptr<MessageHandler>;
    void reset();
    bool is_awaiting_session_id() const;
    bool has_session_id() const;
};

#endif // HAND_SHAKER_H
