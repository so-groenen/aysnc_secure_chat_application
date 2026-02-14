#include "bubble_delegate.h"


//
static constexpr int BO {55};    //BubbleOffset
static constexpr int BD {15};    //BubbleDefaultPadding
static constexpr int TO {BO+10}; //TextOffset
static constexpr int TD {BD+10}; //TextDefault

// QMargin defines a set of four margins:
//                                  left,top,right,bottom, that describe the size of the borders surrounding a rectangle.
static QMargins BUBBLE_PADDING_LEFT {BD, 5,     BO,     5};
static QMargins BUBBLE_PADDING_RIGHT{BO, 5,     BD,     5};
static QMargins TEXT_PADDING_LEFT   {TD, 15,    TO,    15};
static QMargins TEXT_PADDING_RIGHT  {TO, 15,    TD,    15};

static QMargins TEXT_PADDING{25, 15, 25, 15};

BubbleDelegate::BubbleDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void BubbleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    QVariant msg_data             = index.model()->data(index, Qt::DisplayRole);
    Q_ASSERT(msg_data.canConvert<FormattedMessage>());

    auto formatted_msg = qvariant_cast<FormattedMessage>(msg_data);


    const QString& user    = formatted_msg.username();
    const QString& content = formatted_msg.content();
    QColor color           = formatted_msg.color();
    bool is_user           = formatted_msg.is_current_user();


    QString text     = is_user? std::move(content) : user + ":\n" + content;
    auto bubble_rect = is_user? option.rect.marginsRemoved(BUBBLE_PADDING_RIGHT) : option.rect.marginsRemoved(BUBBLE_PADDING_LEFT);
    auto text_rect   = is_user? option.rect.marginsRemoved(TEXT_PADDING_RIGHT)   : option.rect.marginsRemoved(TEXT_PADDING_LEFT) ;
    QPoint p0        = is_user? bubble_rect.topRight() : bubble_rect.topLeft();

    // //   p0+(-20,0)   p0     p0 +(+20,0)
    // //            \         /
    // //             \      /
    // //              \    /
    // //               \  /
    // //           p0+(0,20)

    std::array<QPoint, 3> triangle =
        {
            p0 + QPoint(-20, 0),
            p0 + QPoint(20, 0),
            p0 + QPoint(0, 20)
        };

    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->drawRoundedRect(bubble_rect, 10, 10);
    painter->drawPolygon(triangle.data(), triangle.size());

    painter->setPen(Qt::black);
    painter->drawText(text_rect, Qt::TextWrapAnywhere, text);
}




static constexpr int DEFAULT_WIDTH{400};
static constexpr int TEXT_RECT_HEIGHT{10000};

QSize BubbleDelegate::sizeHint(const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    QVariant msg_data = index.model()->data(index, Qt::DisplayRole);
    Q_ASSERT(msg_data.canConvert<FormattedMessage>());

    auto formatted_msg = qvariant_cast<FormattedMessage>(msg_data);

    const QString& user    = formatted_msg.username();
    const QString& content = formatted_msg.content();
    bool is_user           = formatted_msg.is_current_user();

    QString text          = is_user ? content : user + ":\n" + content;
    QMargins text_padding = is_user ? TEXT_PADDING_RIGHT : TEXT_PADDING_LEFT;

    const QWidget* widget = option.widget;
    int available_width   = widget ? widget->width() : DEFAULT_WIDTH;
    int text_width        = available_width - text_padding.left() - text_padding.right();

    QFontMetricsF metrics{option.font};

    QRectF text_rect = metrics.boundingRect(QRectF(0, 0, text_width, TEXT_RECT_HEIGHT), Qt::TextWrapAnywhere, text);

    QSize final_size = text_rect.toRect()
                          .marginsAdded(text_padding)
                          .size();

    return final_size;
}

