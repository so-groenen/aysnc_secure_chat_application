#include "colored_label.h"


ColoredLabel::ColoredLabel(QWidget *parent)
    : QLabel{parent}
{

}

void ColoredLabel::setColor(QColor color)
{
    QPalette current_palette = palette();
    current_palette.setColor(QPalette::WindowText, color);
    setAutoFillBackground(true);
    setPalette(current_palette);
}
