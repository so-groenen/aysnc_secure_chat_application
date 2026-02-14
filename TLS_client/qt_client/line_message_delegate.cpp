#include "line_message_delegate.h"

//                               left, top, right, bottom
static QMargins TEXT_PADDING_LEFT {25, 5, 45, 5};
static QMargins TEXT_PADDING_RIGHT{45, 5, 25, 5};


LineMessageDelegate::LineMessageDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{
}

static constexpr int MAX_BUBBLE_WIDTH_RATIO{65}; // percent
static constexpr int BUBBLE_RADIUS{12};
static constexpr int VERTICAL_MARGIN{6};

void LineMessageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    QVariant msg_data = index.model()->data(index, Qt::DisplayRole);
    Q_ASSERT(msg_data.canConvert<FormattedMessage>());

    auto formatted_msg = qvariant_cast<FormattedMessage>(msg_data);

    const QString& user    = formatted_msg.username();
    const QString& content = formatted_msg.content();
    QColor color           = formatted_msg.color();
    bool is_user           = formatted_msg.is_current_user();

    QString text   = is_user? std::move(content) : user + ": " + content;
    auto text_rect = is_user? option.rect.marginsRemoved(TEXT_PADDING_RIGHT) : option.rect.marginsRemoved(TEXT_PADDING_LEFT) ;
    int alignement = is_user? Qt::AlignRight                                 : Qt::AlignLeft;

    painter->setPen(color);
    painter->drawText(text_rect, alignement | Qt::TextWrapAnywhere, text);
}

QSize LineMessageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant msg_data = index.model()->data(index, Qt::DisplayRole);
    Q_ASSERT(msg_data.canConvert<FormattedMessage>());

    auto formatted_msg = qvariant_cast<FormattedMessage>(msg_data);

    const QString& user    = formatted_msg.username();
    const QString& content = formatted_msg.content();
    bool is_user           = formatted_msg.is_current_user();

    QString text = is_user ? content: user + ": " + content;

    QMargins text_padding = is_user ? TEXT_PADDING_RIGHT : TEXT_PADDING_LEFT;

    int available_width = option.widget->width();
    int text_width      = available_width - text_padding.left() - text_padding.right();

    auto metrics    = QFontMetricsF{option.font};
    auto bounding   = metrics.boundingRect(QRectF(0, 0, text_width, 0), Qt::TextWordWrap,text);

    QSize size      = bounding.toRect().size();
    size.rwidth()  += text_padding.left() + text_padding.right();
    size.rheight() += text_padding.top() + text_padding.bottom();

    return size;
}












