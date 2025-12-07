#ifndef FORMATTED_MESSAGE_H
#define FORMATTED_MESSAGE_H
#include <QString>
#include <QColor>
#include <QVariant>

class FormattedMessage
{
    int m_is_current_user{}; // should be an enum: "User, Other, Server or Warning"
    QString m_username{""};
    QString m_content{""};
    QColor m_color{};
public:
    // rule of 3 for the QMetatype
    FormattedMessage() = default;
    FormattedMessage(const FormattedMessage &) = default;
    FormattedMessage& operator=(const FormattedMessage &) = default;
    ~FormattedMessage() = default;

    explicit FormattedMessage(QString user, QString msg, QColor color, bool is_current_user);
    const QString& content() const;
    const QString& username() const;
    const QColor& color() const;
    bool is_current_user() const;
};

Q_DECLARE_METATYPE(FormattedMessage);


#endif // FORMATTED_MESSAGE_H
