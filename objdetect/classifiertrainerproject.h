#ifndef CLASSIFIERTRAINERPROJECT_H
#define CLASSIFIERTRAINERPROJECT_H

#include "QList"
#include "QMultiMap"
#include "Section.h"

class QStringList;
class QRect;
class QString;

class ClassifierTrainerProject
{

public:
    ClassifierTrainerProject(QString projectDirectory);
    ~ClassifierTrainerProject();

    QString* name();
    QString* description();
    QString* author();
    QString* type();

    QMultiMap<QString, Section>* positives();
    QList<QString>* negatives();

    bool addPositiveImage(QString pathToPositiveImage);
    bool addNegativeImage(QString pathToNegativeImage);
    bool removePositiveImage(QString pathToPositiveImage);
    bool removeNegativeImage(QString pathToNegativeImage);

    bool addSectionToPositiveImage(QString positiveImagePath, QRect section);
    bool removeSectionFromPositiveImage(QString positiveImagePath, QRect section);

    bool save();
    bool load();
    bool hasErrors();

    void appendToErrors(QString msg);
    QStringList* errors() const;

    static int projectVersionNumber() { return 1; }
    static QString projectType() { return QString("Cascade Classifier Trainer"); }

private:
    QString m_projectDirectory;         // path where the project is found
    QString m_projectConfigFilePath;    // path where the configuration file in the project is found

    QString* m_projectName;
    QString* m_projectDescription;
    QString* m_projectAuthor;
    QString* m_projectType;

    QMultiMap<QString, Section>* m_positives;
    QList<QString>* m_negatives;

    QStringList *m_errors;

    void determine_path_to_config_file(QString projectPath);

};

#endif // CLASSIFIERTRAINERPROJECT_H
