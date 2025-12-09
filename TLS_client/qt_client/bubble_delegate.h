#ifndef BUBBLE_DELEGATE_H
#define BUBBLE_DELEGATE_H

#include <QStyledItemDelegate>
#include <QStyledItemDelegate>
#include <QVariant>
#include <QMargins>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <array>
#include <QApplication>
#include <QtAssert>
#include "formatted_message.h"

class BubbleDelegate : public QStyledItemDelegate
{
public:
    explicit BubbleDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};
#endif // BUBBLE_DELEGATE_H
