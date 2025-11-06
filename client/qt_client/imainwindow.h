#ifndef IMAINWINDOW_H
#define IMAINWINDOW_H

#include <QMainWindow>

class IMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    // explicit IMainWindow(QWidget *parent = nullptr);

public slots:
    virtual void on_connectBtn_clicked() = 0;
signals:
};

#endif // IMAINWINDOW_H
