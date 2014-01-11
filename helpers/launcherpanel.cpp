#include "launcherpanel.h"
#include "ui_launcherpanel.h"
#include "mainwindow.h"
#include "objdetect/gui/classifiertrainergui.h"

LauncherPanel::LauncherPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LauncherPanel)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);
}

LauncherPanel::~LauncherPanel()
{
    delete ui;
}

void LauncherPanel::on_cascadeClassifierTrainerBtn_clicked()
{
    MainWindow *parent = (MainWindow *) this->parent();
    ClassifierTrainerGUI *trainer = new ClassifierTrainerGUI(true, parent);
    parent->setCentralWidget(trainer);
}

void LauncherPanel::on_cascadeClassifierTrainerBtn_2_clicked()
{
    MainWindow *parent = (MainWindow *) this->parent();
    ClassifierTrainerGUI *trainer = new ClassifierTrainerGUI(false, parent);
    parent->setCentralWidget(trainer);
}
