#include "includes.h"
#include "objdetect/mouseselecteventfilter.h"
#include "addsectiondialog.h"
#include "ui_addsectiondialog.h"

AddSectionDialog::AddSectionDialog(QString imagePath, QWidget *parent) :
    QDialog(parent), m_selectionStarted(false), ui(new Ui::AddSectionDialog), m_pathToImage(imagePath),
    m_x(0), m_y(0), m_width(0), m_height(0)
{
    ui->setupUi(this);

    if (imagePath.isEmpty())
    {
        QMessageBox::critical(this, "Invalid Image", "The image you wanted to create a section for is empty.");
        close();
        return;
    }

    if (!QFileInfo(imagePath).exists())
    {
        QMessageBox::critical(this, "Invalid Image", "The image you wanted to create a section for does not exist.");
        close();
        return;
    }

    MouseSelectEventFilter *filter = new MouseSelectEventFilter(this);
    m_image = QPixmap(imagePath);
    ui->imageLbl->installEventFilter(filter);
    ui->imageLbl->setPixmap(m_image);
    ui->imageLbl->setAlignment(Qt::AlignLeft);

    // set pen
    m_pen = QPen(Qt::red);
    m_pen.setWidth(3);

}

AddSectionDialog::~AddSectionDialog()
{
    delete ui;
}

void AddSectionDialog::mouseClick(int x, int y)
{
    m_selectionStarted = !m_selectionStarted;
    ui->resetSectionbtn->setEnabled(!m_selectionStarted);

    if (m_selectionStarted)
        ui->startedIndicator->setText("Yes");
    else
        ui->startedIndicator->setText("No");

    if (m_selectionStarted)
    {
        // selection didn't start, so we're storing X and Y
        m_x = x;
        m_y = y;
        m_width = 0;
        m_height = 0;

        // draw section
        m_image = QPixmap(m_pathToImage);
        QPainter painter(&m_image);
        painter.setPen(m_pen);
        painter.drawPoint(x, y);
        ui->imageLbl->setPixmap(m_image);


    }
    else
    {
        if ((x > m_image.width()) || (y > m_image.height()))
        {
            QMessageBox::warning(this, "Invalid Selection", "The selection you made is out of bounds. Your selection has been reset.");
            on_resetSectionbtn_clicked();
        }
        else
        {
            // selection already started so we need to get the width and height
            if (x < m_x)
            {
                int tmpx = x;
                x = m_x;
                m_x = tmpx;
            }

            if (y < m_y)
            {
                int tmpy = y;
                y = m_y;
                m_y = tmpy;
            }

            m_width = x - m_x;
            m_height = y - m_y;

            // draw section
            m_image = QPixmap(m_pathToImage);
            QPainter painter(&m_image);
            painter.setPen(m_pen);
            painter.drawRect(m_x, m_y, m_width, m_height);
            ui->imageLbl->setPixmap(m_image);
        }
    }

    // update ui
    if (m_x > 0) ui->xVal->setText(QString::number(m_x)); else ui->xVal->setText("N/A");
    if (m_y > 0) ui->yVal->setText(QString::number(m_y)); else ui->yVal->setText("N/A");
    if (m_width > 0) ui->widthVal->setText(QString::number(m_width)); else ui->widthVal->setText("N/A");
    if (m_height > 0) ui->heightVal->setText(QString::number(m_height)); else ui->heightVal->setText("N/A");
}

void AddSectionDialog::on_buttonBox_accepted()
{

}

void AddSectionDialog::on_buttonBox_rejected()
{

}

void AddSectionDialog::on_resetSectionbtn_clicked()
{
    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;
    m_selectionStarted = false;

    // update ui
    if (m_x > 0) ui->xVal->setText(QString::number(m_x)); else ui->xVal->setText("N/A");
    if (m_y > 0) ui->yVal->setText(QString::number(m_y)); else ui->yVal->setText("N/A");
    if (m_width > 0) ui->widthVal->setText(QString::number(m_width)); else ui->widthVal->setText("N/A");
    if (m_height > 0) ui->heightVal->setText(QString::number(m_height)); else ui->heightVal->setText("N/A");
    ui->resetSectionbtn->setEnabled(!m_selectionStarted);

    // update image
    m_image = QPixmap(m_pathToImage);
    ui->imageLbl->setPixmap(m_image);
}

Section AddSectionDialog::section()
{
    return Section(m_pathToImage, m_x, m_y, m_width, m_height);
}
