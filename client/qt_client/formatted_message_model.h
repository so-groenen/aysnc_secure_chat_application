#ifndef FORMATTED_MESSAGE_MODEL_H
#define FORMATTED_MESSAGE_MODEL_H

#include <QAbstractListModel>
#include "formatted_message.h"
#include <QColor>

class FormattedMessageListModel : public QAbstractListModel
{
    QList<FormattedMessage> m_messages{};
public:
    explicit FormattedMessageListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void add_message(const FormattedMessage& msg);
    void add_unformatted_message(const QString& msg);
    void add_warning(const QString& msg);

    void clear_all();
};

#endif // FORMATTED_MESSAGE_MODEL_H
