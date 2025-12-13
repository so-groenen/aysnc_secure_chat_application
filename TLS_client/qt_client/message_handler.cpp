#include "message_handler.h"

static constexpr const char* FROM {"from"};
static constexpr const char* BODY {"body"};
static constexpr const char* ID   {"id"};
static constexpr const char* COLOR{"color"};


MessageHandler::MessageHandler(const QString& username, QColor color, qint64 session_id)
    : m_username{username}, m_color{color}, m_session_id{session_id}
{
    qDebug() << "Message Handler init";
}

QString MessageHandler::parse_to_send(const QString& msg) const
{
    QJsonObject json_msg{};
    json_msg[FROM]  = m_username;
    json_msg[BODY]  = msg;
    json_msg[ID]    = m_session_id;
    json_msg[COLOR] = static_cast<qint64>(m_color.rgb());

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

    auto content         = json_msg.value(BODY).toString();
    auto user            = json_msg.value(FROM).toString();
    auto user_session_id = json_msg.value(ID).toInteger();    //qint64
    auto color_rgb       = json_msg.value(COLOR).toInteger(); //qint64
    auto color           = QColor::fromRgb(static_cast<QRgb>(color_rgb)); // qint64 -> unsigned int(QRGB) -> color


    qDebug() << "MESSAGE HANDLER: Parse to receive:";
    qDebug() << content;
    qDebug() << user;
    qDebug() << user_session_id;
    qDebug() << color;

    bool is_current_user {user_session_id == m_session_id};
    FormattedMessage parsed_msg{user, content, color, is_current_user};

    return parsed_msg;
}

void MessageHandler::set_font_color(QColor color)
{
    m_color = color;
}

QColor MessageHandler::get_font_color() const
{
    return m_color;
}

MessageHandler::~MessageHandler()
{
    qDebug() << "Message Handler destroyed";
}


