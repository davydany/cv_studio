#ifndef SECTION_H
#define SECTION_H

class Section
{
public:
    Section() : m_path(""), m_x(0), m_y(0), m_width(0), m_height(0)
    { }

    Section(QString path, unsigned int x, unsigned int y, unsigned int width, unsigned int height) :
        m_path(path), m_x(x), m_y(y), m_width(width), m_height(height)
    { }

    int x() { return m_x; }
    int y() { return m_y; }
    int width() { return m_width; }
    int height() { return m_height; }
    QString path() { return m_path; }


private:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    QString m_path;

};

#endif // SECTION_H
