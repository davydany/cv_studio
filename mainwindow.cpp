#include <QDebug>
#include <QApplication>
#include "objdetect/gui/classifiertrainergui.h"
#include "helpers/launcherpanel.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)//, projectMenu(0), projectNewCascadeClassifierTrainer(0), projectNewVideoViewer(0), projectNewCameraViewer(0), projectQuit(0)
{
    initialize_menus();
    initialize_central_widget();
}

MainWindow::~MainWindow()
{

}

void MainWindow::initialize_menus()
{
    // project menu ///////////////////////////////////////////////////////////////////////////////////////////////////
    projectMenu = menuBar()->addMenu(tr("&Project"));

    // project >> new
    projectNewMenu = projectMenu->addMenu(tr("&New"));
    projectNewCascadeClassifierTrainer = projectNewMenu->addAction(tr("Cascade Classifier Trainer"));
    projectNewMenu->addSeparator();
    projectNewCameraViewer = projectNewMenu->addAction(tr("Camera Viewer"));
    projectNewVideoViewer = projectNewMenu->addAction(tr("Video Viewer"));


    // project >> open
    projectOpenMenu = projectMenu->addMenu(tr("&Open"));
    projectOpenCascadeClassifierTrainer = projectOpenMenu->addAction(tr("Cascade Classifier Trainer"));
    projectOpenMenu->addSeparator();
    projectOpenCameraViewer = projectOpenMenu->addAction(tr("Camera Viewer"));
    projectOpenVideoViewer = projectOpenMenu->addAction(tr("Video Viewer"));

    // back to project menu
    projectQuit = projectMenu->addAction(tr("&Quit"));

    connect(projectNewCascadeClassifierTrainer, SIGNAL(triggered()),
            this, SLOT(project_new_cascade_classifier_trainer_slot()));
    connect(projectNewCameraViewer, SIGNAL(triggered()), this, SLOT(project_new_camera_viewer_slot()));
    connect(projectNewVideoViewer, SIGNAL(triggered()), this, SLOT(project_new_video_viewer_slot()));
    connect(projectOpenCascadeClassifierTrainer, SIGNAL(triggered()),
            this, SLOT(project_open_cascade_classifier_trainer_slot()));
    connect(projectOpenCameraViewer, SIGNAL(triggered()), this, SLOT(project_open_camera_viewer_slot()));
    connect(projectOpenVideoViewer, SIGNAL(triggered()), this, SLOT(project_open_video_viewer_slot()));
    connect(projectQuit, SIGNAL(triggered()), this, SLOT(project_quit()));
}

void MainWindow::initialize_central_widget()
{
    LauncherPanel *panel = new LauncherPanel(this);
    setCentralWidget(panel);
}


// SLOTS //////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::project_new_cascade_classifier_trainer_slot()
{
    ClassifierTrainerGUI *trainer = new ClassifierTrainerGUI(true, this);
    setCentralWidget(trainer);
}

void MainWindow::project_new_camera_viewer_slot()
{

}

void MainWindow::project_new_video_viewer_slot()
{

}

void MainWindow::project_quit()
{
    QApplication::quit();
}

void MainWindow::project_open_cascade_classifier_trainer_slot()
{
    ClassifierTrainerGUI *trainer = new ClassifierTrainerGUI(false);
    setCentralWidget(trainer);
}

void MainWindow::project_open_video_viewer_slot()
{

}

void MainWindow::project_open_camera_viewer_slot()
{

}

