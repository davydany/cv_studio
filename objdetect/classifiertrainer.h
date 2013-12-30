#ifndef CLASSIFIERTRAINER_H
#define CLASSIFIERTRAINER_H

#include <QWidget>
class QPushButton;
class QTreeView;
class QGroupBox;
class QProgressBar;

class ClassifierTrainer : public QWidget
{
    Q_OBJECT
public:
    explicit ClassifierTrainer(bool new_trainer=true);
    ~ClassifierTrainer();

private:
    // methods
    void initialize();
    void closeTrainer();

    // members
    QGroupBox *positivesGroup;
    QProgressBar *positivesProgressBar;
    QTreeView *positivesList;
    QPushButton *addImageToPositivesBtn;
    QPushButton *delImageFromPositivesBtn;
    QPushButton *addSelectionToPositivesBtn;
    QPushButton *delSelectionToNegativesBtn;

    QGroupBox *negativesGroup;
    QProgressBar *negativesProgressBar;
    QTreeView *negativesList;
    QPushButton *addImageToNegativesBtn;
    QPushButton *delImageFromNegativesBtn;
    QPushButton *addSelectionToNegativesBtn;
    QPushButton *delSelectionFromNegativesBtn;

    QGroupBox *previewPaneGroup;


    QString projectName;
    QString projectSavePath;

    // slots
private slots:

    void add_image_to_positives_slot();
    void del_image_from_positives_slot();
    void add_selection_to_positives_slot();
    void del_selection_from_positives_slot();
    void add_image_to_negatives_slot();
    void del_image_from_negatives_slot();
    void add_selection_to_negatives_slot();
    void del_selection_from_negatives_slot();

};

#endif // CLASSIFIERTRAINER_H
