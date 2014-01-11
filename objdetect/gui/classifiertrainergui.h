#ifndef CLASSIFIERTRAINERGUI_H
#define CLASSIFIERTRAINERGUI_H

#include <QWidget>

class QPushButton;
class QTreeView;
class QLabel;
class QGroupBox;
class QProgressBar;
class QStandardItemModel;
class QModelIndex;
class ClassifierTrainerProject;
class Section;

namespace Ui {
class ClassifierTrainerGUI;
}

class ClassifierTrainerGUI : public QWidget
{
    Q_OBJECT

public:
    explicit ClassifierTrainerGUI(bool new_trainer, QWidget *parent = 0);
    ~ClassifierTrainerGUI();

private:
    Ui::ClassifierTrainerGUI *ui;

    // methods
    void initialize();
    void closeTrainer();
    bool determineProjectConfigFile();
    void updatePositivesGroup();
    void updateNegativesGroup();
    void previewImage(QLabel *imageLabel, QString path, Section *section=0);

    // members
    QTreeView *positivesTreeView;
    QStandardItemModel *positivesModel;
    QPushButton *addImageToPositivesBtn;
    QPushButton *delImageFromPositivesBtn;
    QPushButton *addSelectionToPositivesBtn;
    QPushButton *delSelectionToNegativesBtn;

    QTreeView *negativesTreeView;
    QStandardItemModel *negativesModel;
    QPushButton *addImageToNegativesBtn;
    QPushButton *delImageFromNegativesBtn;

    QGroupBox *previewPaneGroup;
    QLabel *posImageLbl;
    QLabel *negImageLbl;
    QProgressBar *progressBar;

    QString projectName;
    QString projectDirectory;
    QString projectConfigPath;
    ClassifierTrainerProject *project;

private slots:
    void on_addImageToPositivesBtn_clicked();
    void on_delImageFromPositivesBtn_clicked();
    void on_addSelectionToPositivesBtn_clicked();
    void on_delSelectionToNegativesBtn_clicked();
    void on_addImageToNegativesBtn_clicked();
    void on_delImageFromNegativesBtn_clicked();
    void on_positiveTreeView_activated(const QModelIndex &index);
    void on_negativeTreeView_activated(const QModelIndex &index);
    void on_closeTrainer_clicked();
    void on_runTrainer_clicked();
};

#endif // CLASSIFIERTRAINERGUI_H
