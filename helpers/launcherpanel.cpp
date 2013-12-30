#include "launcherpanel.h"
#include "ui_launcherpanel.h"
#include "objdetect/classifiertrainer.h"

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
    ClassifierTrainer *trainer = new ClassifierTrainer(true);
    trainer->show();
}

void LauncherPanel::on_cascadeClassifierTrainerBtn_2_clicked()
{
    ClassifierTrainer *trainer = new ClassifierTrainer(false);
    trainer->show();
}
