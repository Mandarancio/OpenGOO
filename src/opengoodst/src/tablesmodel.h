#ifndef TABLESMODEL_H
#define TABLESMODEL_H

#include <QAbstractTableModel>
#include <QList>

#include "report.h"

class TablesModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    QList<Report*> *reports;

public:
    explicit TablesModel(QList<Report*> *, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool insertRows (Report *report, int position, int rows, const QModelIndex &parent);
    bool removeRows(int position, int rows, const QModelIndex &index);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    Report* getReport(QModelIndex);

signals:

public slots:

};

#endif // TABLESMODEL_H
