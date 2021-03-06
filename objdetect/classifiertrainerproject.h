#ifndef CLASSIFIERTRAINERPROJECT_H
#define CLASSIFIERTRAINERPROJECT_H

#include "QList"
#include "QMultiMap"
#include "Section.h"

class QStringList;
class QRect;
class QString;
class Section;

class ClassifierTrainerProject
{

public:
    ClassifierTrainerProject(QString projectDirectory);
    ~ClassifierTrainerProject();


    QMultiMap<QString, Section>* positive_sections();
    QList<QString>* positives();
    QList<QString>* negatives();


    bool addPositiveImage(QString pathToPositiveImage);
    bool addNegativeImage(QString pathToNegativeImage);
    bool removePositiveImage(QString pathToPositiveImage);
    bool removeNegativeImage(QString pathToNegativeImage);

    bool addSectionToPositiveImage(Section section);
    bool removeSectionFromPositiveImage(Section section);

    bool save();
    bool load();
    bool hasErrors();

    bool writePositivesFile();
    bool writeNegativesFile();

    // training methods
    bool createSamples();
    bool trainCascade();

    void appendToErrors(QString msg);
    QStringList* errors() const;

    static int projectVersionNumber() { return 1; }
    static QString projectType() { return QString("Cascade Classifier Trainer"); }

    // getters and setters
    void setName(QString name);
    void setDescription(QString desc);
    void setAuthor(QString author);

    QString name();
    QString description();
    QString author();
    QString type();

    QString positivesFilename();
    void setPositivesFilename();
    QString negativesFilename();
    void setNegativesFilename();

private:
    QString m_projectDirectory;         // path where the project is found
    QString m_projectConfigFilePath;    // path where the configuration file in the project is found

    QString m_projectName;
    QString m_projectDescription;
    QString m_projectAuthor;
    QString m_projectType;

    QString m_positivesFilename;
    QString m_negativesFilename;

    QMultiMap<QString, Section>* m_positive_sections;
    QList<QString>* m_positives;
    QList<QString>* m_negatives;

    QStringList *m_errors;

    void determine_path_to_config_file(QString projectPath);

};

#endif // CLASSIFIERTRAINERPROJECT_H
