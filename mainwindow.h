#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initialize_menus();
    void initialize_central_widget();

private:

    // menus
    QMenu *projectMenu;
    QMenu *projectNewMenu;
    QMenu *projectOpenMenu;

    // menu actions
    QAction *projectNewCascadeClassifierTrainer;
    QAction *projectNewVideoViewer;
    QAction *projectNewCameraViewer;
    QAction *projectOpenCascadeClassifierTrainer;
    QAction *projectOpenVideoViewer;
    QAction *projectOpenCameraViewer;
    QAction *projectQuit;


private slots:

    // project menu >> new's actions
    void project_new_cascade_classifier_trainer_slot();
    void project_new_video_viewer_slot();
    void project_new_camera_viewer_slot();

    // project menu >> open's actions
    void project_open_cascade_classifier_trainer_slot();
    void project_open_video_viewer_slot();
    void project_open_camera_viewer_slot();

    // project menu's actions
    void project_quit();




};

#endif // MAINWINDOW_H
