#ifndef ADDSECTIONDIALOG_H
#define ADDSECTIONDIALOG_H

#include "objdetect/section.h"
#include <QPen>
#include <QDialog>

namespace Ui {
class AddSectionDialog;
}

class AddSectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddSectionDialog(QString imagePath, QWidget *parent = 0);
    ~AddSectionDialog();

    void mouseClick(int x, int y);
    Section section();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_resetSectionbtn_clicked();

private:
    Ui::AddSectionDialog *ui;
    bool m_selectionStarted = false;
    QString m_pathToImage;
    QPixmap m_image;
    QPen m_pen;

    unsigned int m_x, m_y, m_width, m_height;
};

#endif // ADDSECTIONDIALOG_H