#ifndef MOUSESELECTEVENTFILTER_H
#define MOUSESELECTEVENTFILTER_H

#include <QObject>

class MouseSelectEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit MouseSelectEventFilter(QObject *parent = 0);

    bool eventFilter(QObject *dist, QEvent *event);
};

#endif // MOUSESELECTEVENTFILTER_H
