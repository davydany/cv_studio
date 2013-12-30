#ifndef POSITIVEIMAGEMODEL_H
#define POSITIVEIMAGEMODEL_H

#include "includes.h"
#include <QAbstractItemModel>

class PositiveImageModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit PositiveImageModel(QObject *parent = 0);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;


signals:

public slots:

};

#endif // POSITIVEIMAGEMODEL_H
