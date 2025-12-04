#ifndef FORMATTED_MESSAGE_H
#define FORMATTED_MESSAGE_H
#include <QString>

class FormattedMessage
{
    int m_is_current_user{};
    QString m_username{""};
    QString m_content{""};
public:
    explicit FormattedMessage(QString user, QString msg, bool is_current_user);
    const QString& content() const;
    const QString& username() const;
    bool is_current_user() const;
};

#endif // FORMATTED_MESSAGE_H
