#ifndef MY_TEXT_EDIT_H
#define MY_TEXT_EDIT_H

#include <QTextEdit>
#include <QWidget>
#include <QKeyEvent>
#include <QSet>
class MyTextEdit : public QTextEdit
{
    Q_OBJECT
private:
    bool m_shift_down{};
public:
    explicit MyTextEdit(QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
signals:
    void pressEnterEvent();
};

#endif // MY_TEXT_EDIT_H
