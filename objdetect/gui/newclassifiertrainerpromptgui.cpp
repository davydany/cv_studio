#include "includes.h"
#include "newclassifiertrainerpromptgui.h"
#include "ui_newclassifiertrainerprompt.h"

NewClassifierTrainerPromptGUI::NewClassifierTrainerPromptGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewClassifierTrainerPrompt), projectName(""), projectSavePath(""), isStateValid(false)
{
    ui->setupUi(this);
}

NewClassifierTrainerPromptGUI::~NewClassifierTrainerPromptGUI()
{
    delete ui;
}

void NewClassifierTrainerPromptGUI::on_projSavePathBtn_clicked()
{
    projectSavePath = QFileDialog::getExistingDirectory(this, "Project Directory",
                                                        QDir::homePath(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!validPath(projectSavePath))
    {
        QMessageBox::information(this, tr("Project Directory not Empty"),
                                 tr("The directory you selected is not empty. Please use a directory that is empty for this project."),
                                 QMessageBox::Ok);
        isStateValid = false;
        return;
    }
    else
    {
        ui->projectSavePath->setText(projectSavePath);

    }
}

void NewClassifierTrainerPromptGUI::on_buttonBox_accepted()
{
    projectName = ui->projectName->text();
    projectSavePath = ui->projectSavePath->text();

    // check that the name is set, and the project folder exists and is empty
    if (projectName.isEmpty() || projectSavePath.isEmpty())
    {
        QMessageBox::warning(this, tr("Required Fields Empty"),
                             tr("The fields required to create a new project are empty. Please create them before continuing."));

        return;
    }

    // create the folder if it doesn't exist
    QFileInfo projectDir(projectSavePath);
    if (!projectDir.exists())
    {
        QDir().mkdir(projectSavePath);
    }

    // make subdirectories
    QString positivesPath = QString("%1%2%3").arg(projectSavePath).arg(QDir::separator()).arg("positives");
    QString negativesPath = QString("%1%2%3").arg(projectSavePath).arg(QDir::separator()).arg("negatives");
    QDir().mkdir(positivesPath);
    QDir().mkdir(negativesPath);

    // create project file
    QString projectNameSlug = projectName.toLower().replace(" ", "-");
    QString projectFilePath = QString("%1%2%3.cvst").arg(projectSavePath).arg(QDir::separator()).arg(projectNameSlug);

    QFile file(projectFilePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, "Read Only", "Unable to create the project file.");
        return;
    }
    else
    {
        QXmlStreamWriter *xmlWriter = new QXmlStreamWriter();
        xmlWriter->setDevice(&file);

        // begin writing
        xmlWriter->writeStartDocument();
        xmlWriter->writeStartElement("CVStudioProject");
        xmlWriter->writeAttribute("schema_version", "1.0");

        // write header
        xmlWriter->writeStartElement("Header");
        xmlWriter->writeStartElement("Project");
        xmlWriter->writeTextElement("Name", projectName);
        xmlWriter->writeEmptyElement("Description");
        xmlWriter->writeEmptyElement("Author");
        xmlWriter->writeTextElement("Type", "Cascade Classifier Trainer");

        xmlWriter->writeEndElement(); // end Project
        xmlWriter->writeEndElement(); // end Header

        // create empty positives and negatives nodes
        xmlWriter->writeStartElement("Elements");
        xmlWriter->writeEmptyElement("Positives");
        xmlWriter->writeEmptyElement("Negatives");
        xmlWriter->writeEndElement(); // end Elements

        // close up and clean up
        xmlWriter->writeEndElement(); // end CVStudioProject
        xmlWriter->writeEndDocument(); // end document
        delete xmlWriter;
    }
    if(file.isOpen()) file.close();
}

bool NewClassifierTrainerPromptGUI::validPath(QString path)
{
    QFileInfo fileInfo(path);
    if (fileInfo.isDir())
    {
        QDir dir(path);
        QStringList dirContents = dir.entryList();
        int count = 0;
        for(int i=0; i < dirContents.count(); i++)
        {
            const QString obj = dirContents.at(i);
            if ((QString::compare(obj, ".", Qt::CaseInsensitive) != 0) &&
                    (QString::compare(obj, "..", Qt::CaseInsensitive) != 0))
                count++;
        }
        return (count == 0);
    }
    else
    {
        return false;
    }
}

void NewClassifierTrainerPromptGUI::on_buttonBox_rejected()
{

}

QString NewClassifierTrainerPromptGUI::getProjectName()
{
    return projectName;
}

QString NewClassifierTrainerPromptGUI::getProjectSavePath()
{
    return projectSavePath;
}
