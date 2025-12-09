#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "formatted_message.h"
#include <expected>
#include <QColor>

class MessageHandler
{
    QString m_username{""};
    qint64 m_session_id{};
    QColor m_color{Qt::blue};
public:
    explicit MessageHandler(const QString& username, QColor color, qint64 session_id);
    QString parse_to_send(const QString& msg) const;
    auto parse_to_receive(const QString& msg) const -> std::expected<FormattedMessage, QString>;
    void set_font_color(QColor color);
    QColor get_font_color() const;

    ~MessageHandler();
};

#endif // MESSAGE_HANDLER_H
