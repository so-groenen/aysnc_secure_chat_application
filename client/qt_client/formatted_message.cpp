#include "message.h"

Message::Message(QString user, QString msg, bool is_current_user)
    : m_username{user}, m_content{msg}, m_is_current_user{is_current_user}
{
}

const QString &Message::content() const
{
    return m_content;
}

const QString &Message::username() const
{
    return m_username;
}

bool Message::is_current_user() const
{
    return m_is_current_user;
}
