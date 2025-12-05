#include "message_handler.h"

MessageHandler::MessageHandler(const QString& username, QColor color, qint64 session_id)
    : m_username{username}, m_color{color}, m_session_id{session_id}
{
    qDebug() << "Message Handler init";
}

QString MessageHandler::parse_to_send(const QString& msg) const
{
    QJsonObject json_msg{};
    json_msg["from"]  = m_username;
    json_msg["body"]  = msg;
    json_msg["id"]    = m_session_id;

    QJsonObject json_color{};
    json_color["r"] = m_color.red();
    json_color["g"] = m_color.green();
    json_color["b"] = m_color.blue();

    json_msg["color"] = json_color;

    return QString{QJsonDocument{json_msg}.toJson(QJsonDocument::Compact)};
}

auto MessageHandler::parse_to_receive(const QString& msg) const -> std::expected<FormattedMessage, QString>
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

    auto color_json    = json_msg.value("color").toObject();
    int r = color_json.value("r").toInt();
    int g = color_json.value("g").toInt();
    int b = color_json.value("b").toInt();

    QColor color{r,g,b};

    qDebug() << "MESSAGE HANDLER: Parse to receive:";
    qDebug() << content;
    qDebug() << user;
    qDebug() << user_session_id;
    qDebug() << color;

    bool is_current_user {user_session_id == m_session_id};
    FormattedMessage parsed_msg{user, content, color, is_current_user};

    return parsed_msg;
}

MessageHandler::~MessageHandler()
{
    qDebug() << "Message Handler destoyred";
}


