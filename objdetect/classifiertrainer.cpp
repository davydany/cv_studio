#include "includes.h"
#include "objdetect/newclassifiertrainerprompt.h"
#include "objdetect/classifiertrainerproject.h"
#include "classifiertrainer.h"

QString DEFAULT_QFILEDIALOG_IMAGE_FILTER("Image Files (*.png *.jpg *.bmp)");

ClassifierTrainer::ClassifierTrainer(bool new_trainer) :
    QWidget(0)
{
    positivesModel = new QStandardItemModel();

    if (new_trainer)
    {
        NewClassifierTrainerPrompt *prompt = new NewClassifierTrainerPrompt(this);
        prompt->setModal(true);
        int accepted = prompt->exec();
        if (accepted == 1)
        {
            projectDirectory = prompt->getProjectSavePath();
            project = new ClassifierTrainerProject(projectDirectory);
            project->load();
            projectName = *project->name();
        }
        else
        {
            closeTrainer();
            return;
        }
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

            project = new ClassifierTrainerProject(selectedProjectDir);
            project->load();
            if (project->hasErrors())
            {
                QStringList *errors = project->errors();
                for(int i=0; i < errors->count(); i++)
                    QMessageBox::critical(this, "Unable to Open Project", errors->at(i));
                closeTrainer();
            }

            projectName = *project->name();
            projectDirectory = selectedProjectDir;

            // debug
            const QList<QString> keys = project->positives()->keys();
            qDebug() << "Number of positives: " << keys.length();

            QMultiMap<QString, Section> *positives = project->positives();
            for(int i=0; i < keys.length(); i++)
            {
                QString key = keys.at(i);
                QList<Section> values = project->positives()->values();
                qDebug() << "Path: " << key;
                for(int j=0; j < values.length(); j++)
                {
                    Section section = values.at(j);
                    qDebug() << "Section X: " << section.x() << "; Y: " << section.y() <<
                                "; Width: " << section.width() << "; Height: " << section.height();
                }
            }

            const QList<QString> *negatives = project->negatives();
            qDebug() << "Number of Negatives: " << negatives->count();
            for(int i=0; i < negatives->count(); i++)
            {
                qDebug() << negatives->at(i);
            }

            isValid = true;

        }
    }
    initialize();
}

ClassifierTrainer::~ClassifierTrainer()
{
    delete positivesModel; // TODO: delete all newly created variables
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
    positivesTreeView = new QTreeView(positivesGroup);
    updatePositivesGroup();

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
    positivesGroupLayout->addWidget(positivesTreeView);

    positivesGroup->setLayout(positivesGroupLayout);

    // construct negatives section
    negativesGroup = new QGroupBox("Negatives", this);
    negativesProgressBar = new QProgressBar(this);
    negativesProgressBar->hide();
    QHBoxLayout *negativesControlLayout = new QHBoxLayout;
    QVBoxLayout *negativesGroupLayout = new QVBoxLayout;
    negativesTreeView = new QTreeView(this);

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
    negativesGroupLayout->addWidget(negativesTreeView);

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

void ClassifierTrainer::updatePositivesGroup()
{
    project->load(); // get the latest from the file system

    // remove existing model if it exists
    if (positivesModel) delete positivesModel;

    // create model
    positivesModel = new QStandardItemModel();
    positivesModel->setColumnCount(5);
    positivesModel->setHeaderData(0, Qt::Horizontal, "Image", Qt::DisplayRole);
    positivesModel->setHeaderData(1, Qt::Horizontal, "X", Qt::DisplayRole);
    positivesModel->setHeaderData(2, Qt::Horizontal, "Y", Qt::DisplayRole);
    positivesModel->setHeaderData(3, Qt::Horizontal, "Width", Qt::DisplayRole);
    positivesModel->setHeaderData(4, Qt::Horizontal, "Height", Qt::DisplayRole);

    QStandardItem *invisibleRootNode = positivesModel->invisibleRootItem();
    QList<QString> keys = project->positives()->keys();
    for(int i=0; i < keys.length(); i++)
    {
        QString path = keys.at(i);
        QStandardItem *imageRowItem = new QStandardItem(QString(path));
        imageRowItem->setEditable(false);
        imageRowItem->setIcon(QIcon(":/assets/img/image.png"));
        QList<Section> sections = project->positives()->values(path);
        for(int j=0; j < sections.length(); j++)
        {
            Section s = sections.at(j);

            // first column
            QString firstColumn = QString("Section %1").arg(j+1);
            QStandardItem *firstColumnItem = new QStandardItem(firstColumn);
            firstColumnItem->setIcon(QIcon(":/assets/img/shading.png"));
            firstColumnItem->setEditable(false);

            // second column
            QStandardItem *secondColumnItem = new QStandardItem(QString::number(s.x()));
            secondColumnItem->setEditable(false);

            // third column
            QStandardItem *thirdColumnItem = new QStandardItem(QString::number(s.y()));
            thirdColumnItem->setEditable(false);

            // fourth column
            QStandardItem *fourthColumnItem = new QStandardItem(QString::number(s.width()));
            fourthColumnItem->setEditable(false);

            // fifth column
            QStandardItem *fifthColumnItem = new QStandardItem(QString::number(s.width()));
            fifthColumnItem->setEditable(false);

            QList<QStandardItem *> sectionRowItems;
            sectionRowItems.append(firstColumnItem);
            sectionRowItems.append(secondColumnItem);
            sectionRowItems.append(thirdColumnItem);
            sectionRowItems.append(fourthColumnItem);
            sectionRowItems.append(fifthColumnItem);

            imageRowItem->appendRow(sectionRowItems);
        }
        invisibleRootNode->appendRow(imageRowItem);
    }

    positivesTreeView->setModel(positivesModel);
    positivesTreeView->resizeColumnToContents(0);
    positivesTreeView->resizeColumnToContents(1);
    positivesTreeView->resizeColumnToContents(2);
    positivesTreeView->resizeColumnToContents(3);
    positivesTreeView->resizeColumnToContents(4);
}

void ClassifierTrainer::closeTrainer()
{
    this->hide();
//    this->close(); // close if the user requested the window to be closed (NOT WORKING AS INTENDED)
}


void ClassifierTrainer::add_image_to_positives_slot()
{
    QStringList newImages = QFileDialog::getOpenFileNames(this, tr("Positive Image"),
                                                     projectDirectory,
                                                     tr(DEFAULT_QFILEDIALOG_IMAGE_FILTER.toUtf8().constData()));

    QString destDirPath = QString("%1%2%3").arg(projectDirectory).arg(QDir::separator()).arg("positives");
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
        project->addPositiveImage(dstImageAbsPath);

        // update progress bar
        positivesProgressBar->setValue(i+1);
    }

    project->save();
    positivesProgressBar->hide();
    updatePositivesGroup();
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
                                                     projectDirectory, tr(DEFAULT_QFILEDIALOG_IMAGE_FILTER.toUtf8().constData()));
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
