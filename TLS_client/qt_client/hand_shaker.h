#ifndef HAND_SHAKER_H
#define HAND_SHAKER_H
#include "interface_tcp_view.h"
#include "interface_ssl_presenter.h"
#include "interface_ssl_client_model.h"

#include <QString>
#include <memory>
#include "formatted_message.h"
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


    ISslClientModel* m_model{};
    ISslView* m_view{};
    qint64 m_session_id{-1};
    HandShakeMode m_handshake_mode{HandShakeMode::AwaitHandShake};
    ConnectionResult m_connection_result{};
public:
    explicit HandShaker(ISslClientModel* tcp_model, ISslView* tcp_view);
    Result parse_session_id(const QString &message);
    void await_handshake(const ConnectionResult& connection_result);
    auto make_message_handler() const -> std::unique_ptr<MessageHandler>;
    void reset();
    bool is_awaiting_session_id() const;
    bool has_session_id() const;
};

#endif // HAND_SHAKER_H
