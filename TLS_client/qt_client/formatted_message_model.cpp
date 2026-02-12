#include "formatted_message_model.h"

FormattedMessageListModel::FormattedMessageListModel(QObject *parent)
    : QAbstractListModel{parent}
{
}


int FormattedMessageListModel::rowCount(const QModelIndex &parent) const
{
    return static_cast<int>(m_messages.size());
}

QVariant FormattedMessageListModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::DisplayRole)
        return{};

    FormattedMessage msg = m_messages.at(index.row());
    QVariant var;
    var.setValue(msg);

    return var;
}
QVariant FormattedMessageListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant{};
}

void FormattedMessageListModel::add_message(const FormattedMessage &msg)
{
    m_messages.append(msg);
    emit layoutChanged();
}

void FormattedMessageListModel::add_unformatted_message(const QString &msg)
{
    FormattedMessage formatted_msg{"Server", msg, Qt::gray, false};
    m_messages.append(formatted_msg);
    emit layoutChanged();
}

void FormattedMessageListModel::add_warning(const QString &msg)
{
    FormattedMessage warning{"", msg, Qt::red, true};
    m_messages.append(warning);
    emit layoutChanged();
}

void FormattedMessageListModel::clear_all()
{
    m_messages.clear();
    emit layoutChanged();
}
