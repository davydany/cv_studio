#include "includes.h"
#include "objdetect/gui/addsectiondialoggui.h"
#include "objdetect/gui/newclassifiertrainerpromptgui.h"
#include "objdetect/classifiertrainerproject.h"
#include "objdetect/section.h"
#include "mainwindow.h"
#include "classifiertrainergui.h"
#include "ui_classifiertrainergui.h"

QString DEFAULT_QFILEDIALOG_IMAGE_FILTER("Image Files (*.png *.jpg *.bmp)");

ClassifierTrainerGUI::ClassifierTrainerGUI(bool new_trainer, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassifierTrainerGUI)
{
    ui->setupUi(this);
    positivesModel = new QStandardItemModel();
    negativesModel = new QStandardItemModel();

    if (new_trainer)
    {
        NewClassifierTrainerPromptGUI *prompt = new NewClassifierTrainerPromptGUI(this);
        prompt->setModal(true);
        int accepted = prompt->exec();
        if (accepted == 1)
        {
            projectDirectory = prompt->getProjectSavePath();
            project = new ClassifierTrainerProject(projectDirectory);
            project->load();
            projectName = project->name();
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

            projectName = project->name();
            projectDirectory = selectedProjectDir;
            isValid = true;

        }
    }
    initialize();
}

ClassifierTrainerGUI::~ClassifierTrainerGUI()
{
    delete ui;
    delete positivesModel;
    delete negativesModel;
    delete project;
}

void ClassifierTrainerGUI::initialize()
{
    // set the pointers to their ui counterparts
    positivesTreeView = ui->positiveTreeView;
    addImageToPositivesBtn = ui->addImageToPositivesBtn;
    delImageFromPositivesBtn = ui->delImageFromPositivesBtn;
    addSelectionToPositivesBtn = ui->addSelectionToPositivesBtn;
    delSelectionToNegativesBtn = ui->delSelectionToNegativesBtn;

    negativesTreeView = ui->negativeTreeView;
    addImageToNegativesBtn = ui->addImageToNegativesBtn;
    delImageFromNegativesBtn = ui->delImageFromNegativesBtn;

    posImageLbl = ui->posImageLbl;
    negImageLbl = ui->negImageLbl;
    progressBar = ui->progressBar;

    // set layouts
    setLayout(ui->mainLayout);
    ui->positiveTab->setLayout(ui->positiveTabMainLayout);
    ui->positiveImagesGroup->setLayout(ui->positiveImagesGroupLayout);
    ui->positiveImagePreviewGroup->setLayout(ui->positiveImagePreviewGroupLayout);
    ui->positiveTab->adjustSize();

    ui->negativeTab->setLayout(ui->negativeTabMainLayout);
    ui->negativeImageGroup->setLayout(ui->negativeImageGroupLayout);
    ui->negativeImagePreviewGroup->setLayout(ui->negativeImagePreviewGroupLayout);
    ui->negativeTab->adjustSize();

    ui->prefTab->setLayout(ui->prefTabLayout);
    ui->consoleOutputPage->setLayout(ui->consoleOutputPageLayout);

    // misc
    setWindowTitle(QString("Classifier Trainer - ") + project->name());
    MainWindow *parent = (MainWindow *) this->parent();
    parent->setWindowTitle(windowTitle());
    updatePositivesGroup();
    updateNegativesGroup();
    ui->progressBar->hide();

    // load project components to ui
    ui->projName->setText(project->name());
    ui->projDescription->setText(project->description());
    ui->projAuthor->setText(project->author());

}
void ClassifierTrainerGUI::updatePositivesGroup()
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
    ui->positiveImageCount->setText(QString::number(keys->length()));
    for(int i=0; i < keys->length(); i++)
    {
        QString path = keys->at(i);
        QStandardItem *imageRowItem = new QStandardItem(path);
        imageRowItem->setEditable(false);
        imageRowItem->setIcon(QIcon(":/image.png"));

        QList<Section> sections = project->positive_sections()->values(path);
        for(int j=0; j < sections.length(); j++)
        {
            Section s = sections.at(j);

            // first column
            QString firstColumn = path;
            QStandardItem *firstColumnItem = new QStandardItem(firstColumn);
            firstColumnItem->setIcon(QIcon(":/shading.png"));
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

void ClassifierTrainerGUI::updateNegativesGroup()
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

void ClassifierTrainerGUI::previewImage(QLabel *imageLabel, QString path, Section *s)
{
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
            imageLabel->setPixmap(pixmap);
            imageLabel->setFixedSize(pixmap.width(), pixmap.height());
        }
        else
        {
            QMessageBox::critical(this, "Invalid Image", "The selected image does not exist anymore.");
            return;
        }
    }
    else
    {
        imageLabel->setText("Select an Image to Preview");
    }

//    QScrollArea *scrollArea = new QScrollArea(this);
//    scrollArea->setWidget(imageLabel);

//    QVBoxLayout *layout = new QVBoxLayout(this);
//    layout->addWidget(scrollArea);

//    if (previewPaneGroup->layout() != 0)
//    {
//        QVBoxLayout *existingLayout = (QVBoxLayout *) previewPaneGroup->layout();
//        delete existingLayout;
//    }

//    previewPaneGroup->setLayout(layout);


}

void ClassifierTrainerGUI::closeTrainer()
{
    this->hide();
//    this->close(); // close if the user requested the window to be closed (NOT WORKING AS INTENDED)
}

void ClassifierTrainerGUI::log(QString string)
{
    ui->consoleOutBox->appendPlainText(string);
}

void ClassifierTrainerGUI::updateProgressBar(int percent)
{
    if ((percent >= 0) && (percent <= 100))
        ui->progressBar->setValue(percent);
}

void ClassifierTrainerGUI::prepareRun()
{
    ui->progressBar->show();
    ui->progressBar->setValue(0);
    ui->consoleOutBox->clear();
}

void ClassifierTrainerGUI::cleanRun()
{
    ui->progressBar->hide();
}

// SLOTS --------------------------------------------------------------------------------------------------------------
void ClassifierTrainerGUI::on_addImageToPositivesBtn_clicked()
{
    QStringList newImages = QFileDialog::getOpenFileNames(this, tr("Positive Image"),
                                                     projectDirectory,
                                                     tr(DEFAULT_QFILEDIALOG_IMAGE_FILTER.toUtf8().constData()));

    QString destDirPath = QString("%1%2%3").arg(projectDirectory).arg(QDir::separator()).arg("positives");
    QDir dest(destDirPath);
    progressBar->setMaximum(newImages.length());
    progressBar->show();
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
        progressBar->setValue(i+1);
    }

    project->save();
    progressBar->hide();
    updatePositivesGroup();
}

void ClassifierTrainerGUI::on_delImageFromPositivesBtn_clicked()
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

void ClassifierTrainerGUI::on_addSelectionToPositivesBtn_clicked()
{
    QModelIndexList indices = positivesTreeView->selectionModel()->selectedIndexes();
    if (indices.length() > 0)
    {
        QModelIndex index = indices[0];
        QStandardItem *item = positivesModel->item(index.row());
        AddSectionDialogGUI *dialog = new AddSectionDialogGUI(item->text(), this);
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

void ClassifierTrainerGUI::on_delSelectionToNegativesBtn_clicked()
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

void ClassifierTrainerGUI::on_addImageToNegativesBtn_clicked()
{
    QStringList newImages = QFileDialog::getOpenFileNames(this, tr("Negative Image"),
                                          projectDirectory, tr(DEFAULT_QFILEDIALOG_IMAGE_FILTER.toUtf8().constData()));

    QString destDirPath = QString("%1%2%3").arg(projectDirectory).arg(QDir::separator()).arg("negatives");
    QDir dest(destDirPath);
    progressBar->setMaximum(newImages.length());
    progressBar->show();
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
        progressBar->setValue(i+1);
    }

    project->save();
    progressBar->hide();
    updateNegativesGroup();
}

void ClassifierTrainerGUI::on_delImageFromNegativesBtn_clicked()
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

void ClassifierTrainerGUI::on_positiveTreeView_activated(const QModelIndex &index)
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
        previewImage(ui->posImageLbl, path, &s);
    }
    else
    {
        previewImage(ui->posImageLbl, path);
    }
}

void ClassifierTrainerGUI::on_negativeTreeView_activated(const QModelIndex &index)
{
    QStandardItem *item = negativesModel->item(index.row());
    QString path = item->text();
    previewImage(ui->negImageLbl, path);
}

void ClassifierTrainerGUI::on_closeTrainer_clicked()
{
    closeTrainer();
}



void ClassifierTrainerGUI::on_runTrainer_clicked()
{
    prepareRun();
    log("Starting training process ...");
    ui->progressBar->show();
    updateProgressBar(0);

    // save the positives images and sections
    bool success = project->writePositivesFile();
    if (success)
    {
        log(QString("Saved Positives Images to 'positives.dat'"));
        updateProgressBar(5);
    }
    else
    {
        foreach(QString err, *project->errors())
        {
            log(QString("Error: ") + err);
            QMessageBox::critical(this, "Unable to save positives images to file.", err);
        }
        return cleanRun();
    }

    // save the negatives images
    success = project->writeNegativesFile();
    if (success)
    {
        log(QString("Saved Negative Images to 'negatives.dat'"));
        updateProgressBar(10);
    }
    else
    {
        foreach(QString err, *project->errors())
        {
            log(QString("Error: ") + err);
            QMessageBox::critical(this, "Unable to save negative images to file.", err);
        }
        return cleanRun();
    }

    // start create_samples



    // start training
}

void ClassifierTrainerGUI::on_projName_editingFinished()
{
    if (project->name().compare(ui->projName->text()) != 0)
    {
        project->setName(ui->projName->text());
        project->save();
        ui->projSavedLabel->setText("Project Name was successfully saved.");
    }
}

void ClassifierTrainerGUI::on_projDescription_editingFinished()
{
    if (project->description().compare(ui->projDescription->text()) != 0)
    {
        project->setDescription(ui->projDescription->text());
        project->save();
        ui->projSavedLabel->setText("Project Description was successfully saved.");
    }
}

void ClassifierTrainerGUI::on_projAuthor_editingFinished()
{
    if (project->author().compare(ui->projAuthor->text()) != 0)
    {
        project->setAuthor(ui->projAuthor->text());
        project->save();
        ui->projSavedLabel->setText("Project Author was successfully saved.");
    }
}
