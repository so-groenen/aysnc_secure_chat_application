#include "formatted_message.h"

FormattedMessage::FormattedMessage(QString user, QString msg, QColor color, bool is_current_user)
    : m_username{user}, m_content{msg}, m_color{color}, m_is_current_user{is_current_user}
{
}

const QString &FormattedMessage::content() const
{
    return m_content;
}

const QString &FormattedMessage::username() const
{
    return m_username;
}

const QColor &FormattedMessage::color() const
{
    return m_color;
}

bool FormattedMessage::is_current_user() const
{
    return m_is_current_user;
}


