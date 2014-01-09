#include "includes.h"
#include "objdetect/addsectiondialog.h"
#include "objdetect/newclassifiertrainerprompt.h"
#include "objdetect/classifiertrainerproject.h"
#include "objdetect/section.h"
#include "classifiertrainer.h"

QString DEFAULT_QFILEDIALOG_IMAGE_FILTER("Image Files (*.png *.jpg *.bmp)");

ClassifierTrainer::ClassifierTrainer(bool new_trainer) :
    QWidget(0)
{
    positivesModel = new QStandardItemModel();
    negativesModel = new QStandardItemModel();

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
            isValid = true;

        }
    }
    initialize();
}

ClassifierTrainer::~ClassifierTrainer()
{
    delete positivesModel; // TODO: delete all newly created variables
    delete negativesModel;
    delete positivesGroup;
    delete positivesProgressBar;
    delete positivesTreeView;
    delete positivesModel;
    delete addImageToPositivesBtn;
    delete delImageFromPositivesBtn;
    delete addSelectionToPositivesBtn;
    delete delSelectionToNegativesBtn;
    delete negativesGroup;
    delete negativesProgressBar;
    delete negativesTreeView;
    delete negativesModel;
    delete addImageToNegativesBtn;
    delete delImageFromNegativesBtn;
    delete previewPaneGroup;
    delete previewImageLabel;
    delete project;
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
    delImageFromPositivesBtn->setShortcut(QKeySequence(tr("Ctrl+D")));
    addSelectionToPositivesBtn = new QPushButton(QIcon(":/note_add.png"), "Add Section", this);
    delSelectionToNegativesBtn = new QPushButton(QIcon(":/note_delete.png"), "Delete Section", this);

    connect(addImageToPositivesBtn, SIGNAL(clicked()), this, SLOT(add_image_to_positives_slot()));
    connect(delImageFromPositivesBtn, SIGNAL(clicked()), this, SLOT(del_image_from_positives_slot()));
    connect(addSelectionToPositivesBtn, SIGNAL(clicked()), this, SLOT(add_selection_to_positives_slot()));
    connect(delSelectionToNegativesBtn, SIGNAL(clicked()), this, SLOT(del_selection_from_positives_slot()));
    connect(positivesTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(show_image_preview_for_positive_item(QModelIndex)));

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
    updateNegativesGroup();

    addImageToNegativesBtn = new QPushButton(QIcon(":/image_add.png"), "Add Image", this);
    delImageFromNegativesBtn = new QPushButton(QIcon(":/image_delete.png"), "Delete Image", this);
    delImageFromNegativesBtn->setShortcut(QKeySequence(tr("Ctrl+E")));

    connect(addImageToNegativesBtn, SIGNAL(clicked()), this, SLOT(add_image_to_negatives_slot()));
    connect(delImageFromNegativesBtn, SIGNAL(clicked()), this, SLOT(del_image_from_negatives_slot()));
    connect(negativesTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(show_image_preview_for_negative_item(QModelIndex)));

    negativesControlLayout->addWidget(addImageToNegativesBtn);
    negativesControlLayout->addWidget(delImageFromNegativesBtn);

    negativesGroupLayout->addLayout(negativesControlLayout);
    negativesGroupLayout->addWidget(negativesProgressBar);
    negativesGroupLayout->addWidget(negativesTreeView);

    negativesGroup->setLayout(negativesGroupLayout);

    // create the preview plane
    previewPaneGroup = new QGroupBox("Image Preview", this);
    previewPaneGroup->setFixedWidth(500);
    previewImage("");

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

    // create column headers for the model
    positivesModel = new QStandardItemModel();
    positivesModel->setColumnCount(5);
    positivesModel->setHeaderData(0, Qt::Horizontal, "Image", Qt::DisplayRole);
    positivesModel->setHeaderData(1, Qt::Horizontal, "X", Qt::DisplayRole);
    positivesModel->setHeaderData(2, Qt::Horizontal, "Y", Qt::DisplayRole);
    positivesModel->setHeaderData(3, Qt::Horizontal, "Width", Qt::DisplayRole);
    positivesModel->setHeaderData(4, Qt::Horizontal, "Height", Qt::DisplayRole);

    QStandardItem *invisibleRootNode = positivesModel->invisibleRootItem();
    QList<QString> *keys = project->positives();
    qDebug() << "Number of positives loaded: " << keys->length();
    for(int i=0; i < keys->length(); i++)
    {
        QString path = keys->at(i);
        QStandardItem *imageRowItem = new QStandardItem(path);
        imageRowItem->setEditable(false);
        imageRowItem->setIcon(QIcon(":/assets/img/image.png"));

        QList<Section> sections = project->positive_sections()->values(path);
        for(int j=0; j < sections.length(); j++)
        {
            Section s = sections.at(j);

            // first column
            QString firstColumn = path;
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
            QStandardItem *fifthColumnItem = new QStandardItem(QString::number(s.height()));
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
    positivesTreeView->setColumnWidth(0, 350);
    positivesTreeView->resizeColumnToContents(1);
    positivesTreeView->resizeColumnToContents(2);
    positivesTreeView->resizeColumnToContents(3);
    positivesTreeView->resizeColumnToContents(4);
    positivesTreeView->expandAll();
}

void ClassifierTrainer::updateNegativesGroup()
{
    project->load(); // get the latest from the file system

    // remove existing model if it exists
    if (negativesModel) delete negativesModel;

    // create column headers for the model
    negativesModel = new QStandardItemModel();
    negativesModel->setColumnCount(1);
    negativesModel->setHeaderData(0, Qt::Horizontal, "Image", Qt::DisplayRole);

    // populate the rows
    QStandardItem *invisibleRootNode = negativesModel->invisibleRootItem();
    QList<QString> *negatives = project->negatives();
    for(int i=0; i < negatives->length(); i++)
    {
        QString path = negatives->at(i);
        QStandardItem *imageRowItem = new QStandardItem(QString(path));
        imageRowItem->setEditable(false);
        imageRowItem->setIcon(QIcon(":/assets/img/image.png"));
        invisibleRootNode->appendRow(imageRowItem);
    }

    negativesTreeView->setModel(negativesModel);
    negativesTreeView->resizeColumnToContents(0);

}

void ClassifierTrainer::previewImage(QString path, Section *s)
{
    previewImageLabel = new QLabel();
    if (!path.isEmpty())
    {
        if (QFile(path).exists())
        {
            QPixmap pixmap(path);
            // draw section if we get section info
            if (s)
            {
                QPen pen(Qt::red);
                pen.setWidth(3);
                QPainter painter(&pixmap);
                painter.setPen(pen);
                painter.drawRect(s->x(), s->y(), s->width(), s->height());
            }

            // draw pixmap
            previewImageLabel->setPixmap(pixmap);
        }
        else
        {
            QMessageBox::critical(this, "Invalid Image", "The selected image does not exist anymore.");
            return;
        }
    }
    else
    {
        previewImageLabel->setText("Select an Image to Preview");
    }

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(previewImageLabel);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(scrollArea);

    if (previewPaneGroup->layout() != 0)
    {
        QVBoxLayout *existingLayout = (QVBoxLayout *) previewPaneGroup->layout();
        delete existingLayout;
    }

    previewPaneGroup->setLayout(layout);

}

void ClassifierTrainer::closeTrainer()
{
    this->hide();
//    this->close(); // close if the user requested the window to be closed (NOT WORKING AS INTENDED)
}




// SLOTS //////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        QString dstImageAbsPath = QString("%1%2%3").arg(destDirPath).arg(QDir::separator())
                .arg(srcImageFileInfo.fileName());

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
    QModelIndexList indices = positivesTreeView->selectionModel()->selectedIndexes();
    if (indices.length() > 0)
    {
        QModelIndex index = indices[0];
        project->load();
        QStandardItem *item = positivesModel->itemFromIndex(index);
        if (!item->parent())
        {
            project->removePositiveImage(item->text());
            project->save();
            updatePositivesGroup();
        }
        else
        {
            // TODO
            qDebug() << "Not parent. Need to get to parent.";
        }
    }

}
void ClassifierTrainer::add_selection_to_positives_slot()
{
    QModelIndexList indices = positivesTreeView->selectionModel()->selectedIndexes();
    if (indices.length() > 0)
    {
        QModelIndex index = indices[0];
        QStandardItem *item = positivesModel->item(index.row());
        AddSectionDialog *dialog = new AddSectionDialog(item->text(), this);
        dialog->setModal(true);
        int response = dialog->exec();

        if (response == 1) // accepted
        {
            Section s = dialog->section();
            int sectionRowCount = item->rowCount() + 1;

            // save contents to project config file
            project->load();
            if (project->addSectionToPositiveImage(s))
            {
                project->save();

                // update ui
                updatePositivesGroup();
            }
        }
    }
}
void ClassifierTrainer::del_selection_from_positives_slot()
{
    // section
    QModelIndexList selectedIndices = positivesTreeView->selectionModel()->selectedIndexes();
    QString path = positivesModel->itemFromIndex(selectedIndices[0])->text();
    bool xOK = false, yOK = false, widthOK = false, heightOK = false;
    int x = positivesModel->itemFromIndex(selectedIndices[1])->text().toInt(&xOK),
        y = positivesModel->itemFromIndex(selectedIndices[2])->text().toInt(&yOK),
        width = positivesModel->itemFromIndex(selectedIndices[3])->text().toInt(&widthOK),
        height = positivesModel->itemFromIndex(selectedIndices[4])->text().toInt(&heightOK);

    if (xOK && yOK && widthOK && heightOK)
    {
        Section s(path, x, y, width, height);
        project->load();
        if (project->removeSectionFromPositiveImage(s))
        {
            project->save();

            // update ui
            updatePositivesGroup();
        }
    }
    else
    {
        QMessageBox::warning(this, "Invalid Selection", "The item you selected is not a valid section and so it cannot be deleted.");
    }
}
void ClassifierTrainer::add_image_to_negatives_slot()
{
    QStringList newImages = QFileDialog::getOpenFileNames(this, tr("Negative Image"),
                                          projectDirectory, tr(DEFAULT_QFILEDIALOG_IMAGE_FILTER.toUtf8().constData()));

    QString destDirPath = QString("%1%2%3").arg(projectDirectory).arg(QDir::separator()).arg("negatives");
    QDir dest(destDirPath);
    negativesProgressBar->setMaximum(newImages.length());
    negativesProgressBar->show();
    for(int i=0; i < newImages.length(); i++)
    {
        // initialize
        QString srcImageAbsPath = newImages.at(i);
        QFileInfo srcImageFileInfo(srcImageAbsPath);
        QString dstImageAbsPath = QString("%1%2%3").arg(destDirPath).arg(
                    QDir::separator()).arg(srcImageFileInfo.fileName());

        // copy the image from src directory to dest directory
        if (QFile::exists(dstImageAbsPath))
            QFile::remove(dstImageAbsPath);
        QFile::copy(srcImageAbsPath, dstImageAbsPath);

        // add record to xml file
        project->addNegativeImage(dstImageAbsPath);

        // update progress bar
        negativesProgressBar->setValue(i+1);
    }

    project->save();
    negativesProgressBar->hide();
    updateNegativesGroup();

}
void ClassifierTrainer::del_image_from_negatives_slot()
{
    QModelIndexList indices = negativesTreeView->selectionModel()->selectedIndexes();
    if (indices.length() > 0)
    {
        QModelIndex index = indices[0];
        project->load();

        QStandardItem *item = negativesModel->itemFromIndex(index);
        qDebug() << "ABout to delete negative : " << item->text();
        project->removeNegativeImage(item->text());
        project->save();
        updateNegativesGroup();
    }

}

void ClassifierTrainer::show_image_preview_for_positive_item(const QModelIndex &index)
{
    // section
    QModelIndexList selectedIndices = positivesTreeView->selectionModel()->selectedIndexes();
    QString path = positivesModel->itemFromIndex(selectedIndices[0])->text();
    bool xOK = false, yOK = false, widthOK = false, heightOK = false;
    int x = positivesModel->itemFromIndex(selectedIndices[1])->text().toInt(&xOK),
        y = positivesModel->itemFromIndex(selectedIndices[2])->text().toInt(&yOK),
        width = positivesModel->itemFromIndex(selectedIndices[3])->text().toInt(&widthOK),
        height = positivesModel->itemFromIndex(selectedIndices[4])->text().toInt(&heightOK);

    if (xOK && yOK && widthOK && heightOK)
    {
        Section s(path, x, y, width, height);
        previewImage(path, &s);
    }
    else
    {
        previewImage(path);
    }
}

void ClassifierTrainer::show_image_preview_for_negative_item(const QModelIndex &index)
{
    QStandardItem *item = negativesModel->item(index.row());
    QString path = item->text();
    previewImage(path);
}
