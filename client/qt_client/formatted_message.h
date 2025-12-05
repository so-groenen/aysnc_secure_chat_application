#ifndef FORMATTED_MESSAGE_H
#define FORMATTED_MESSAGE_H
#include <QString>
#include <QColor>

class FormattedMessage
{
    int m_is_current_user{};
    QString m_username{""};
    QString m_content{""};
    QColor m_color{};
public:
    explicit FormattedMessage(QString user, QString msg, QColor color, bool is_current_user);
    const QString& content() const;
    const QString& username() const;
    const QColor& color() const;
    bool is_current_user() const;
};

#endif // FORMATTED_MESSAGE_H
