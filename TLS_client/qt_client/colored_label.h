#ifndef COLORED_LABEL_H
#define COLORED_LABEL_H

#include <QLabel>
#include <QColor>
#include <QWidget>
#include <QPalette>

class ColoredLabel : public QLabel
{
public:
    explicit ColoredLabel(QWidget *parent= nullptr);
    void setColor(QColor color);
};

#endif // COLORED_LABEL_H
