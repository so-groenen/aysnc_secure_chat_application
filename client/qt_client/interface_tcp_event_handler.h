#ifndef INTERFACE_TCP_EVENT_HANDLER_H
#define INTERFACE_TCP_EVENT_HANDLER_H
#include <QString>
#include <expected>
#include <optional>
#include <variant>
#include "formatted_message.h"


// When an event occurs in TCP Client, the TCP client needs an event handler
// The presenters goal is to act as a layer to abstract the GUI and perform Message Formatting & error handling
// For ex, when a message arrives, the TCP client will call the "handle_msg_reception" of the Presenter
// to forward raw messages.
// The presenter will then in turn call the GUI's "handle_msg_reception" to display formated messages


using ConnectionError    = QString;
using IPAdress           = QString;
using MessageVariant     = std::variant<QString, FormattedMessage>;
using ConnectionResult   = std::expected<IPAdress, ConnectionError>;
using MessageSendFailure = std::optional<QString>;


class ITcpEventHandler
{
public:
    virtual ~ITcpEventHandler() = default;
    virtual void handle_connect_response(const ConnectionResult& connect_result) = 0;
    virtual void handle_msg_reception(const MessageVariant& msg) = 0;
    virtual void handle_msg_send_failure(const MessageSendFailure& failed_msg) = 0;
    virtual void handle_disconnect_from_host() = 0;
};


#endif // INTERFACE_TCP_EVENT_HANDLER_H
