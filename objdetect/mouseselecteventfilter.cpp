#include "includes.h"
#include "mouseselecteventfilter.h"
#include "objdetect/addsectiondialog.h"

MouseSelectEventFilter::MouseSelectEventFilter(QObject *parent) :
    QObject(parent)
{
}


bool MouseSelectEventFilter::eventFilter(QObject *dist, QEvent *event)
{
    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = (QMouseEvent *) event;
        int mouseX = mouseEvent->pos().x();
        int mouseY = mouseEvent->pos().y();

        AddSectionDialog *p = (AddSectionDialog *) parent();
        p->mouseMove(mouseX, mouseY);
    }

    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *mouseEvent = (QMouseEvent *) event;
        int mouseX = mouseEvent->pos().x();
        int mouseY = mouseEvent->pos().y();

        AddSectionDialog *p = (AddSectionDialog *) parent();
        p->mouseClick(mouseX, mouseY);
    }
}
