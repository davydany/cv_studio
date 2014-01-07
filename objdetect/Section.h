#ifndef SECTION_H
#define SECTION_H

#include <QString>

class Section
{
public:
    Section() : m_x(0), m_y(0), m_width(0), m_height(0), m_path("")
    { }

    Section(QString path, unsigned int x, unsigned int y, unsigned int width, unsigned int height) :
        m_path(path), m_x(x), m_y(y), m_width(width), m_height(height)
    { }

    int x() { return this->m_x; }
    int y() { return this->m_y; }
    int width() { return this->m_width; }
    int height() { return this->m_height; }
    QString path() { return m_path; }

    void setPath(QString p) { m_path = p; }
    void setX(int x) { m_x = x; }
    void setY(int y) { m_y = y; }
    void setWidth(int width) { m_width = width; }
    void setHeight(int height) { m_height = height; }

    QString toString()
    {
        QString x = QString::number(m_x),
                y = QString::number(m_y),
                width = QString::number(m_width),
                height = QString::number(m_height);
        return m_path + QString("X: %1; Y: %2; Width: %3; Height: %4").arg(x, y, width, height);
    }

    bool operator==(Section b)
    {
        bool pathComp = (m_path.compare(b.path()) == 0);
        bool xComp = (m_x == b.x());
        bool yComp = (m_y == b.y());
        bool widthComp = (m_width == b.width());
        bool heightComp = (m_height == b.height());

        return (pathComp && xComp && yComp && widthComp && heightComp);
    }

    bool operator!=(Section b)
    {
        bool pathComp = (m_path.compare(b.path()) == 0);
        bool xComp = (m_x == b.x());
        bool yComp = (m_y == b.y());
        bool widthComp = (m_width == b.width());
        bool heightComp = (m_height == b.height());

        return !(pathComp && xComp && yComp && widthComp && heightComp);
    }

private:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    QString m_path;

};

#endif // SECTION_H
