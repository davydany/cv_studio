#ifndef NEWCLASSIFIERTRAINERPROMPT_H
#define NEWCLASSIFIERTRAINERPROMPT_H

#include <QDialog>

namespace Ui {
class NewClassifierTrainerPrompt;
}

class NewClassifierTrainerPromptGUI : public QDialog
{
    Q_OBJECT

public:
    explicit NewClassifierTrainerPromptGUI(QWidget *parent = 0);
    ~NewClassifierTrainerPromptGUI();

    QString getProjectName();
    QString getProjectSavePath();

private:
    Ui::NewClassifierTrainerPrompt *ui;
    QString projectName;
    QString projectSavePath;
    bool isStateValid;

    bool validPath(QString path);

private slots:

    void on_projSavePathBtn_clicked();

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // NEWCLASSIFIERTRAINERPROMPT_H
