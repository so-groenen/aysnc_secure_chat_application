#include "message_handler.h"

MessageHandler::MessageHandler(QString username, qint64 session_id)
    : m_username{username}, m_session_id{session_id}
{
    qDebug() << "Message Handler init";
}

QString MessageHandler::parse_to_send(QString msg) const
{
    QJsonObject json_msg{};
    json_msg["from"] = m_username;
    json_msg["body"] = msg;
    json_msg["id"]   = m_session_id;

    return QString{QJsonDocument{json_msg}.toJson()};
}

auto MessageHandler::parse_to_receive(QString msg) const -> std::expected<FormattedMessage, QString>
{
    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
    if(!doc.isObject())
    {
        return std::unexpected(msg);
    }
    QJsonObject json_msg = doc.object();

    auto content        = json_msg.value("body").toString();
    auto user           = json_msg.value("from").toString();
    auto user_session_id = json_msg.value("id").toInteger(); //qint64

    qDebug() << "MESSAGE HANDLER: Parse to receive:";
    qDebug() << content;
    qDebug() << user;
    qDebug() << user_session_id;

    bool is_current_user {user_session_id == m_session_id};
    FormattedMessage parsed_msg{user, content, is_current_user};

    return parsed_msg;
}

MessageHandler::~MessageHandler()
{
    qDebug() << "Message Handler destoyred";
}


