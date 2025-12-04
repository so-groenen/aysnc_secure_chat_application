#ifndef INTERFACE_TCP_EVENT_HANDLER_H
#define INTERFACE_TCP_EVENT_HANDLER_H
#include <QString>
#include <expected>
#include <optional>
#include <variant>
#include "formatted_message.h"


// When an event occurs in TCP Client, we forward it to the presenter to parse and perform error handling
// THe Presenter will get raw Messages, and format it for the GUI/View
// THe GUI will have its own implementation to display the results

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
