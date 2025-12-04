#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "message.h"


class MessageHandler
{
    QString m_username{""};
    qint64 m_session_id{};
public:
    explicit MessageHandler(QString username, qint64 session_id);
    QString parse_to_send(QString msg) const;
    Message parse_to_receive(QString msg) const;
    ~MessageHandler();
};

#endif // MESSAGE_HANDLER_H
