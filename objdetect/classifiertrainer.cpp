#include "includes.h"
#include "objdetect/newclassifiertrainerprompt.h"
#include "classifiertrainer.h"

QString DEFAULT_QFILEDIALOG_IMAGE_FILTER("Image Files (*.png *.jpg *.bmp)");

ClassifierTrainer::ClassifierTrainer(bool new_trainer) :
    QWidget(0)
{

    if (new_trainer)
    {
        NewClassifierTrainerPrompt *prompt = new NewClassifierTrainerPrompt(this);
        prompt->setModal(true);
        int accepted = prompt->exec();
        if (accepted != 1) {
            closeTrainer();
            return;
        }

        projectName = prompt->getProjectName();
        projectSavePath = prompt->getProjectSavePath();
    }
    else
    {
        bool isValid = false;
        while (!isValid)
        {
            QString selectedProjectDir = QFileDialog::getExistingDirectory(this, "Open Cascade Classifier Training Project", QDir::homePath(),
                                                                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
            // user chose not to select a directory, so we get out
            if (selectedProjectDir.length() == 0)
            {
                closeTrainer();
                return;
            }

            // user chose a directory, so we move on...
            if ( QFileInfo::exists(selectedProjectDir))
            {
                QDir projectDir(selectedProjectDir);
                QStringList filters; filters << "*.cvst";
                QStringList dirListing = projectDir.entryList(filters);
                if (dirListing.count() == 1)
                {
                    isValid = true;
                    QString fileName = dirListing.at(0);
                    QString filePath = QString("%1%2%3")
                            .arg(selectedProjectDir).arg(QDir::separator()).arg(fileName);
                    QFile file(filePath);
                    if(file.open(QIODevice::ReadOnly))
                    {
                        QXmlStreamReader *reader = new QXmlStreamReader();
                        reader->setDevice(&file);
                        reader->readNext();
                        while(!reader->isEndDocument())
                        {
                            if(reader->isStartElement())
                            {
                                QString nodeName = reader->name().toString();
                                if (nodeName == "Name")
                                {
                                    projectName = reader->readElementText();
                                    projectSavePath = selectedProjectDir;
                                    isValid = true;
                                }
                            }
                            reader->readNext();
                        }
                    }
                    else
                    {
                        QMessageBox::critical(this, tr("Unable to open project file."),
                                              tr("The project file in the directory you selected cannot be open for reading."));
                    }
                    if (file.isOpen()) file.close();
                }
                else
                {
                    QMessageBox::critical(this, tr("Invalid Project Folder"),
                                          tr("The folder you selected does not contain a valid .cvst file that describes the project."));
                    closeTrainer();
                    return;
                }
            }
            else
            {
                QMessageBox::critical(this, tr("Invalid Project Folder"),
                                      tr("The folder you selected does not exist."));
                isValid = false;
            }
        }
    }
    initialize();
}

ClassifierTrainer::~ClassifierTrainer()
{

}

void ClassifierTrainer::initialize()
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *imagesListLayout = new QVBoxLayout;

    // construct positives section
    positivesGroup = new QGroupBox("Positives", this);
    positivesProgressBar = new QProgressBar(this);
    positivesProgressBar->hide();
    QHBoxLayout *positivesControlLayout = new QHBoxLayout;
    QVBoxLayout *positivesGroupLayout = new QVBoxLayout;
    positivesList = new QTreeView(positivesGroup);

    addImageToPositivesBtn = new QPushButton(QIcon(":/image_add.png"), "Add Image", this);
    delImageFromPositivesBtn = new QPushButton(QIcon(":/image_delete.png"), "Delete Image", this);
    addSelectionToPositivesBtn = new QPushButton(QIcon(":/note_add.png"), "Add Section", this);
    delSelectionToNegativesBtn = new QPushButton(QIcon(":/note_delete.png"), "Delete Section", this);

    connect(addImageToPositivesBtn, SIGNAL(clicked()), this, SLOT(add_image_to_positives_slot()));
    connect(delImageFromPositivesBtn, SIGNAL(clicked()), this, SLOT(del_image_from_positives_slot()));
    connect(addSelectionToPositivesBtn, SIGNAL(clicked()), this, SLOT(add_selection_to_positives_slot()));
    connect(delSelectionToNegativesBtn, SIGNAL(clicked()), this, SLOT(del_selection_from_positives_slot()));

    positivesControlLayout->addWidget(addImageToPositivesBtn);
    positivesControlLayout->addWidget(delImageFromPositivesBtn);
    positivesControlLayout->addWidget(addSelectionToPositivesBtn);
    positivesControlLayout->addWidget(delSelectionToNegativesBtn);


    positivesGroupLayout->addLayout(positivesControlLayout);
    positivesGroupLayout->addWidget(positivesProgressBar);
    positivesGroupLayout->addWidget(positivesList);

    positivesGroup->setLayout(positivesGroupLayout);

    // construct negatives section
    negativesGroup = new QGroupBox("Negatives", this);
    negativesProgressBar = new QProgressBar(this);
    negativesProgressBar->hide();
    QHBoxLayout *negativesControlLayout = new QHBoxLayout;
    QVBoxLayout *negativesGroupLayout = new QVBoxLayout;
    negativesList = new QTreeView(this);

    addImageToNegativesBtn = new QPushButton(QIcon(":/image_add.png"), "Add Image", this);
    delImageFromNegativesBtn = new QPushButton(QIcon(":/image_delete.png"), "Delete Image", this);
    addSelectionToNegativesBtn = new QPushButton(QIcon(":/note_add.png"), "Add Section", this);
    delSelectionFromNegativesBtn = new QPushButton(QIcon(":/note_delete.png"), "Delete Section", this);

    connect(addImageToNegativesBtn, SIGNAL(clicked()), this, SLOT(add_image_to_negatives_slot()));
    connect(delImageFromNegativesBtn, SIGNAL(clicked()), this, SLOT(del_image_from_negatives_slot()));
    connect(addSelectionToNegativesBtn, SIGNAL(clicked()), this, SLOT(add_selection_to_negatives_slot()));
    connect(delSelectionFromNegativesBtn, SIGNAL(clicked()), this, SLOT(del_selection_from_negatives_slot()));

    negativesControlLayout->addWidget(addImageToNegativesBtn);
    negativesControlLayout->addWidget(delImageFromNegativesBtn);
    negativesControlLayout->addWidget(addSelectionToNegativesBtn);
    negativesControlLayout->addWidget(delSelectionFromNegativesBtn);

    negativesGroupLayout->addLayout(negativesControlLayout);
    negativesGroupLayout->addWidget(negativesProgressBar);
    negativesGroupLayout->addWidget(negativesList);

    negativesGroup->setLayout(negativesGroupLayout);

    // create the preview plane
    previewPaneGroup = new QGroupBox("Image Preview", this);

    // set main layout
    imagesListLayout->addWidget(positivesGroup);
    imagesListLayout->addWidget(negativesGroup);

    mainLayout->addLayout(imagesListLayout);
    mainLayout->addWidget(previewPaneGroup);

    // finalize gui
    setLayout(mainLayout);
    QString title = QString("Cascade Classifier Trainer - %1").arg(projectName);
    setWindowTitle(title);
}

void ClassifierTrainer::closeTrainer()
{
    close(); // close if the user requested the window to be closed (NOT WORKING AS INTENDED)
    hide();
}

void ClassifierTrainer::add_image_to_positives_slot()
{
    QStringList newImages = QFileDialog::getOpenFileNames(this, tr("Positive Image"),
                                                     projectSavePath,
                                                     tr(DEFAULT_QFILEDIALOG_IMAGE_FILTER.toUtf8().constData()));

    QString destDirPath = QString("%1%2%3").arg(projectSavePath).arg(QDir::separator()).arg("positives");
    QDir dest(destDirPath);
    positivesProgressBar->setMaximum(newImages.length());
    positivesProgressBar->show();
    for(int i=0; i < newImages.length(); i++)
    {
        // initialize
        QString srcImageAbsPath = newImages.at(i);
        QFileInfo srcImageFileInfo(srcImageAbsPath);
        QString dstImageAbsPath = QString("%1%2%3").arg(destDirPath).arg(QDir::separator()).arg(srcImageFileInfo.fileName());

        // copy the image from src directory to dest directory
        if (QFile::exists(dstImageAbsPath))
            QFile::remove(dstImageAbsPath);
        QFile::copy(srcImageAbsPath, dstImageAbsPath);

        // add record to xml file

        // update progress bar
        positivesProgressBar->setValue(i+1);
    }
    positivesProgressBar->hide();
}
void ClassifierTrainer::del_image_from_positives_slot()
{
}
void ClassifierTrainer::add_selection_to_positives_slot()
{
}
void ClassifierTrainer::del_selection_from_positives_slot()
{
}
void ClassifierTrainer::add_image_to_negatives_slot()
{
    QStringList newImages = QFileDialog::getOpenFileNames(this, tr("Negative Image"),
                                                     projectSavePath, tr(DEFAULT_QFILEDIALOG_IMAGE_FILTER.toUtf8().constData()));
}
void ClassifierTrainer::del_image_from_negatives_slot()
{
}
void ClassifierTrainer::add_selection_to_negatives_slot()
{
}
void ClassifierTrainer::del_selection_from_negatives_slot()
{
}
