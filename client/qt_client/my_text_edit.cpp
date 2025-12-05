#include "my_text_edit.h"

MyTextEdit::MyTextEdit(QWidget *parent)
    : QTextEdit(parent)
{

}

void MyTextEdit::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Return)
    {
        if(m_shift_down)
        {
            QTextEdit::append("");
            qDebug() << "Shift Enter pressed";
        }
        else
        {
            emit pressEnterEvent();
            qDebug() << "Enter pressed";
        }
        return;
    }

    if(e->key() == Qt::Key_Shift)
    {
        m_shift_down = true;
    }

    QTextEdit::keyPressEvent(e);
}

void MyTextEdit::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Shift && m_shift_down)
    {
        m_shift_down = false;
    }
    QTextEdit::keyPressEvent(e);
}
