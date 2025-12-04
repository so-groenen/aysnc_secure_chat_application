#ifndef MESSAGE_H
#define MESSAGE_H
#include <QString>

class Message
{
    int m_is_current_user{};
    QString m_username{""};
    QString m_content{""};
public:
    explicit Message(QString user, QString msg, bool is_current_user);
    const QString& content() const;
    const QString& username() const;
    bool is_current_user() const;
};

#endif // MESSAGE_H
