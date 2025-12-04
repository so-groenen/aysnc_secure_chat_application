#include "formatted_message.h"

FormattedMessage::FormattedMessage(QString user, QString msg, bool is_current_user)
    : m_username{user}, m_content{msg}, m_is_current_user{is_current_user}
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

bool FormattedMessage::is_current_user() const
{
    return m_is_current_user;
}
