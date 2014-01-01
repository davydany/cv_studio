#include "includes.h"
#include "objdetect/section.h"
#include "classifiertrainerproject.h"

ClassifierTrainerProject::ClassifierTrainerProject(QString projectPath) //:
//    validProjectFile(false), m_projectDirectory(projectDirectory), m_projectConfigFilePath(QString(""))
{
    m_errors = new QStringList();

    m_projectName = new QString();
    m_projectDescription = new QString();
    m_projectAuthor = new QString();
    m_projectType = new QString(ClassifierTrainerProject::projectType());

    m_positives = new QMultiMap<QString, Section>();
    m_negatives = new QList<QString>();

    determine_path_to_config_file(projectPath);
}

ClassifierTrainerProject::~ClassifierTrainerProject()
{
    delete m_projectName;
    delete m_projectDescription;
    delete m_projectAuthor;
    delete m_projectType;

    delete m_positives;
    delete m_negatives;

    delete m_errors;
}

void ClassifierTrainerProject::determine_path_to_config_file(QString projectPath)
{
    QDir projectDir(projectPath);
    if (projectDir.exists())
    {
        QStringList filter; filter << "*.cvst";
        QStringList cvstFiles = projectDir.entryList(filter);
        if (cvstFiles.count() == 1)
        {
            m_projectConfigFilePath = QString("%1%2%3").arg(projectPath).arg(QDir::separator()).arg(cvstFiles.at(0));
        }
        else
        {
            appendToErrors(QString("A single project file could not be found in %1.").arg(projectPath));
        }
    }
    else
    {
        appendToErrors(QString("The specified project folder (%1) does not exist.").arg(projectPath));
    }
}

bool ClassifierTrainerProject::hasErrors()
{
    return m_errors->count() > 0;
}

void ClassifierTrainerProject::appendToErrors(QString msg)
{
    m_errors->append(msg);
}

QStringList* ClassifierTrainerProject::errors() const
{
    return m_errors;
}


QString* ClassifierTrainerProject::name()
{
    return m_projectName;
}
QString* ClassifierTrainerProject::description()
{
    return m_projectDescription;
}
QString* ClassifierTrainerProject::author()
{
    return m_projectAuthor;
}
QString* ClassifierTrainerProject::type()
{
    return m_projectType;
}

QMultiMap<QString, Section> *ClassifierTrainerProject::positives()
{
    return m_positives;
}

QList<QString> *ClassifierTrainerProject::negatives()
{
    return m_negatives;
}

bool ClassifierTrainerProject::addPositiveImage(QString pathToPositiveImage)
{
    if (m_positives)
    {
        if (m_positives->values(pathToPositiveImage).length() == 0)
        {
            QImage image = QImage(pathToPositiveImage);
            Section section = Section(pathToPositiveImage, 0, 0, image.width(), image.height());
            qDebug() << "Adding Image, this is the path: " << pathToPositiveImage;
            m_positives->insert(pathToPositiveImage, section);
        }
    }
}

bool ClassifierTrainerProject::addNegativeImage(QString pathToNegativeImage)
{
    return false;
}

bool ClassifierTrainerProject::removePositiveImage(QString pathToPositiveImage)
{
    return false;
}

bool ClassifierTrainerProject::removeNegativeImage(QString pathToNegativeImage)
{
    return false;
}

bool ClassifierTrainerProject::addSectionToPositiveImage(QString positiveImagePath, QRect section)
{
    return false;
}

bool ClassifierTrainerProject::removeSectionFromPositiveImage(QString positiveImagePath, QRect section)
{
    return false;
}

bool ClassifierTrainerProject::load()
{
    // clean up existing values if they're used
    if (m_positives) delete m_positives;
    if (m_negatives) delete m_negatives;


    QFile configFile(m_projectConfigFilePath);
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        appendToErrors(QString("Unable to open config file in ReadOnly mode.").arg(m_projectConfigFilePath));
        return false;
    }

    QXmlStreamReader reader;
    reader.setDevice(&configFile);
    reader.readNext();

    while(!reader.isEndDocument())
    {
        if(reader.isStartElement())
        {
            QString nodeName = reader.name().toString();
            if (nodeName == "Name")
                m_projectName = new QString(reader.readElementText());

            if (nodeName == "Description")
                m_projectDescription = new QString(reader.readElementText());

            if (nodeName == "Author")
                m_projectAuthor = new QString(reader.readElementText());

            if (nodeName == "Type")
                m_projectType = new QString(reader.readElementText());

            if (nodeName == "Elements")
            {
                reader.readNextStartElement();
                if (reader.name().toString() == "Positives")
                {
                    reader.readNextStartElement();
                    m_positives = new QMultiMap<QString, Section>();
                    while(reader.name().toString() == "Image")
                    {
                        QString path;
                        QXmlStreamAttributes imageAttr = reader.attributes();
                        if (imageAttr.hasAttribute("path"))
                            path = imageAttr.value("path").toString();
                        reader.readNextStartElement();
                        while(reader.name().toString() == "Section")
                        {
                            QString x, y, width, height;
                            QXmlStreamAttributes sectionAttr = reader.attributes();
                            if (sectionAttr.hasAttribute("x")) x = sectionAttr.value("x").toString();
                            if (sectionAttr.hasAttribute("y")) y = sectionAttr.value("y").toString();
                            if (sectionAttr.hasAttribute("width")) width = sectionAttr.value("width").toString();
                            if (sectionAttr.hasAttribute("height")) height = sectionAttr.value("height").toString();

                            if (!(x.isEmpty() && y.isEmpty() && width.isEmpty() && height.isEmpty()))
                            {
                                // @TODO: There needs to be a better XML Parsing than this. This is ridiculous.
                                Section section(path, x.toInt(), y.toInt(), width.toInt(), height.toInt());
                                m_positives->insertMulti(path, section);
                            }
                            reader.readNextStartElement();
                        }
                    }
                }

                reader.readNextStartElement();
                if (reader.name().toString() == "Negatives")
                {
                    reader.readNextStartElement();
                    m_negatives = new QList<QString>();
                    while(reader.name().toString() == "Image")
                    {
                        QString path;
                        QXmlStreamAttributes imageAttr = reader.attributes();
                        if (imageAttr.hasAttribute("path"))
                            path = imageAttr.value("path").toString();

                        if (!path.isEmpty())
                            m_negatives->append(path);
                        reader.readNextStartElement();
                    }
                }
            }
        }
        reader.readNext();
    }
    configFile.close();
}

bool ClassifierTrainerProject::save()
{
    // check that the config file exists before starting
    if (m_projectConfigFilePath.isEmpty())
        determine_path_to_config_file(m_projectConfigFilePath);
        if (m_projectConfigFilePath.isEmpty())
            return false;

    // prepare to save
    QFile configFile(m_projectConfigFilePath);
    QXmlStreamWriter xmlWriter;
    if (!configFile.open(QIODevice::WriteOnly))
    {
        appendToErrors("Unable to complete save() operaiton to config file.");
        return false;
    }
    xmlWriter.setDevice(&configFile);

    // begin writing
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("CVStudioProject");
    xmlWriter.writeAttribute("schema_version", QString::number(ClassifierTrainerProject::projectVersionNumber()));

    // write header
    xmlWriter.writeStartElement("Header");
    xmlWriter.writeStartElement("Project");
    xmlWriter.writeTextElement("Name", QString(name()->toStdString().c_str()));
    xmlWriter.writeTextElement("Description", QString(description()->toStdString().c_str()));
    xmlWriter.writeTextElement("Author", QString(author()->toStdString().c_str()));
    xmlWriter.writeTextElement("Type", ClassifierTrainerProject::projectType());
    xmlWriter.writeEndElement(); // end Project
    xmlWriter.writeEndElement(); // end Header

    // begin writing the elements
    xmlWriter.writeStartElement("Elements"); // start <Elements> tag

    // write the positive elements
    QList<QString> positive_keys = m_positives->keys();
    for(int i=0; i < positive_keys.length(); i++)
    {
        qDebug() << "Positive Key : [" << positive_keys.at(i) << "]";
    }
    if (positive_keys.count() == 0)
        xmlWriter.writeEmptyElement("Positives");
    else
    {
        xmlWriter.writeStartElement("Positives"); // start <Positives> tag
        qDebug() << "Number of Positives: " << positive_keys.length();
        for(int i=0; i < positive_keys.length(); i++)
        {
            QString key = positive_keys.at(i);
            QList<Section> sections = positives()->values(key);
            qDebug() << " Key (" << key << ") at i = " << i << " has " << sections.length() << "sections.";
            if (sections.length() == 0)
            {
                // start and close <Image> tag
                xmlWriter.writeStartElement("Image");
                xmlWriter.writeAttribute("path", key);
                xmlWriter.writeEndElement();
            }
            else
            {
                // begin writing <Image> tag
                xmlWriter.writeStartElement("Image");
                xmlWriter.writeAttribute("path", key);

                // begin writing each <Section> tag for the <Image> tag
                for(int j=0; j < sections.length(); j++)
                {
                    Section section = sections.at(j);
                    xmlWriter.writeStartElement("Section"); // start <Section> tag
                    xmlWriter.writeAttribute("x", QString::number(section.x()));
                    xmlWriter.writeAttribute("y", QString::number(section.y()));
                    xmlWriter.writeAttribute("width", QString::number(section.width()));
                    xmlWriter.writeAttribute("height", QString::number(section.height()));
                    xmlWriter.writeEndElement(); // close <Section> tag
                }

                // close <Image> tag
                xmlWriter.writeEndElement();
            }
        }
        xmlWriter.writeEndElement(); // close <Positives> tag
    }

    // write the negative elements
    QList<QString> *negative_images = negatives();
    if (negative_images->size() == 0)
        xmlWriter.writeEmptyElement("Negatives");
    else
    {
        xmlWriter.writeStartElement("Negatives");   // start <Negatives> tag
        for(int i=0; i < negative_images->length(); i++)
        {
            QString path = negative_images->at(i);
            xmlWriter.writeStartElement("Image"); // start <Image> tag
            xmlWriter.writeAttribute("path", path);
            xmlWriter.writeEndElement(); // close <Image> tag
        }
        xmlWriter.writeEndElement(); // close <Negatives> tag
    }

    // close document
    xmlWriter.writeEndElement(); // close <Elements> tag
    xmlWriter.writeEndElement(); // close <CVStudioProject> tag
    xmlWriter.writeEndDocument(); // close document

}
