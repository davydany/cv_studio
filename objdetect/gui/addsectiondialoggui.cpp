#include "includes.h"
#include "mouseselecteventfilter.h"
#include "addsectiondialoggui.h"
#include "ui_addsectiondialog.h"

AddSectionDialogGUI::AddSectionDialogGUI(QString imagePath, QWidget *parent) :
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

    // calculate the diff
    int diffHeight = height() - ui->imageLbl->height();
    int diffWidth = width() - ui->imageLbl->width();

    m_filter = new MouseSelectEventFilter(this);
    m_image = QPixmap(imagePath);
    ui->imageLbl->installEventFilter(m_filter);
    ui->imageLbl->setPixmap(m_image);
    ui->imageLbl->setAlignment(Qt::AlignLeft);

    // set pen
    m_pen = QPen(QColor(0xFF, 0, 0, 127));
    m_pen.setWidth(2);

    ui->imageLbl->setFixedWidth(m_image.width());
    ui->imageLbl->setFixedHeight(m_image.height());
    ui->scrollArea->setFixedWidth(m_image.width());
    ui->scrollArea->setFixedHeight(m_image.height());

    setLayout(ui->gridLayout);
    adjustSize();
}

AddSectionDialogGUI::~AddSectionDialogGUI()
{
    delete ui;
    delete m_filter;

}

void AddSectionDialogGUI::mouseMove(int x, int y)
{
    if (x < ui->imageLbl->x()) return;
    if (y < ui->imageLbl->y()) return;

    // get label dimension
    int w = ui->imageLbl->width();
    int h = ui->imageLbl->height();

    m_image = QPixmap(m_pathToImage);
    QPainter painter(&m_image);


    if (m_selectionStarted)
    {
        // selection already started so we need to get the width and height
        painter.setPen(m_pen);
        int width = abs(m_x - x);
        int height = abs(m_y - y);
        int trueX = x, trueY = y;
        if (m_x < x) trueX = m_x;
        if (m_y < y) trueY = m_y;

        painter.drawRect(trueX, trueY, width, height);
    }

    if (ui->showGuides->checkState() == Qt::Checked)
    {
        QPen borderPen(QColor(255, 255, 255, 255));
        borderPen.setWidth(3);
        painter.setPen(borderPen);
        painter.drawLine(QPointF(0, y), QPointF(m_image.width(), y)); // draw horizontal line
        painter.drawLine(QPointF(x, 0), QPointF(x, m_image.height())); // draw vertical line

        QPen pen(QColor(0, 0, 0, 127));
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawLine(QPointF(0, y), QPointF(m_image.width(), y)); // draw horizontal line
        painter.drawLine(QPointF(x, 0), QPointF(x, m_image.height())); // draw vertical line
    }

    ui->imageLbl->setPixmap(m_image);
}

void AddSectionDialogGUI::mouseClick(int x, int y)
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
        if (ui->showGuides->checkState() == Qt::Checked)
        {
            m_image = QPixmap(m_pathToImage);
            QPainter painter(&m_image);
            painter.setPen(m_pen);
            painter.drawPoint(x, y);
            ui->imageLbl->setPixmap(m_image);
        }
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
            if (ui->showGuides->checkState() == Qt::Checked)
            {
                m_image = QPixmap(m_pathToImage);
                QPainter painter(&m_image);
                painter.setPen(m_pen);
                painter.drawRect(m_x, m_y, m_width, m_height);
                ui->imageLbl->setPixmap(m_image);
            }
        }
    }

    // update ui
    if (m_x > 0) ui->xVal->setText(QString::number(m_x)); else ui->xVal->setText("N/A");
    if (m_y > 0) ui->yVal->setText(QString::number(m_y)); else ui->yVal->setText("N/A");
    if (m_width > 0) ui->widthVal->setText(QString::number(m_width)); else ui->widthVal->setText("N/A");
    if (m_height > 0) ui->heightVal->setText(QString::number(m_height)); else ui->heightVal->setText("N/A");
}

void AddSectionDialogGUI::on_buttonBox_accepted()
{

}

void AddSectionDialogGUI::on_buttonBox_rejected()
{

}

void AddSectionDialogGUI::on_resetSectionbtn_clicked()
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

    // get label dimension
    int w = ui->imageLbl->width();
    int h = ui->imageLbl->height();

    // update image
    m_image = QPixmap(m_pathToImage);
    ui->imageLbl->setPixmap(m_image);
}

Section AddSectionDialogGUI::section()
{
    return Section(m_pathToImage, m_x, m_y, m_width, m_height);
}

void AddSectionDialogGUI::on_showGuides_clicked()
{
    m_image = QPixmap(m_pathToImage);
    ui->imageLbl->setPixmap(m_image);
}
