#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "formatted_message.h"
#include <expected>


class MessageHandler
{
    QString m_username{""};
    qint64 m_session_id{};
public:
    explicit MessageHandler(QString username, qint64 session_id);
    QString parse_to_send(QString msg) const;
    auto parse_to_receive(QString msg) const -> std::expected<FormattedMessage, QString>;
    ~MessageHandler();
};

#endif // MESSAGE_HANDLER_H
