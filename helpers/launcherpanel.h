#ifndef LAUNCHERPANEL_H
#define LAUNCHERPANEL_H

#include <QWidget>

namespace Ui {
class LauncherPanel;
}

class LauncherPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LauncherPanel(QWidget *parent = 0);
    ~LauncherPanel();

private slots:
    void on_cascadeClassifierTrainerBtn_clicked();

    void on_cascadeClassifierTrainerBtn_2_clicked();

private:
    Ui::LauncherPanel *ui;
};

#endif // LAUNCHERPANEL_H
