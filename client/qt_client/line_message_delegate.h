#ifndef LINE_MESSAGE_DELEGATE_H
#define LINE_MESSAGE_DELEGATE_H

#include <QStyledItemDelegate>
#include <QVariant>
#include <QMargins>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <array>
#include <QApplication>
#include <QtAssert>
#include "formatted_message.h"

class LineMessageDelegate : public QStyledItemDelegate
{
public:
    explicit LineMessageDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // LINE_MESSAGE_DELEGATE_H
