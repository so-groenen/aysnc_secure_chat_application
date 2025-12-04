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

Message MessageHandler::parse_to_receive(QString msg) const
{
    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
    //if(!doc.isObject())
    //{
    // HANDLE ERROR !!
    //}
    QJsonObject json_msg = doc.object();

    QString content        = json_msg.value("body").toString();
    QString user           = json_msg.value("from").toString();
    qint64 user_session_id = json_msg.value("id").toInteger();

    qDebug() << "MESSAGE HANDLER: Parse to receive:";
    qDebug() << content;
    qDebug() << user;
    qDebug() << user_session_id;

    bool is_current_user = (user_session_id == m_session_id);
    Message parsed_msg{user, content, is_current_user};

    return parsed_msg;
}

MessageHandler::~MessageHandler()
{
    qDebug() << "Message Handler destoyred";
}


