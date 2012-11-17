#include "tablesmodel.h"

#include <QDebug>

TablesModel::TablesModel(QList<Report*> *reports, QObject *parent) :
    QAbstractTableModel(parent)
{
    this->reports = reports;
}

int TablesModel::rowCount(const QModelIndex &parent) const {

    Q_UNUSED(parent);
    return reports->size();
}

int TablesModel::columnCount(const QModelIndex &parent) const {

    Q_UNUSED(parent);
    return 6;
}

QVariant TablesModel::data(const QModelIndex &index, int role) const {

    if (!index.isValid())
        return QVariant();

    if (index.row() >= reports->size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {

        Report *report = reports->at(index.row());

        switch(index.column()) {

            case 0:
                return report->getGameVer();

            case 1:
                return report->getDateTime();

            case 2:
                return report->getSendDate();

            case 4:
                return report->getgameVerFix();

            case 5:
                return report->getSent();

            default:
                return QVariant();
        }
    }


    if (role == Qt::DisplayRole || role == Qt::CheckStateRole) {

        Report *report = reports->at(index.row());

        if (index.column() == 3) {

            bool fixedState = report->getFixed();

            switch(fixedState) {

                case true:
                    return 2;

                case false:
                    return 0;
            }
        }
    }

    return QVariant();
}

QVariant TablesModel::headerData(int section, Qt::Orientation orientation, int role) const {

    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {

            case 0:
                return tr("GameVersion");

            case 1:
                return tr("Date");

            case 2:
                return tr("Send Date");

            case 3:
                return tr("Fixed");

            case 4:
                return tr("in Version");

            default:
                return QVariant();
        }
    }

    return QVariant();
}

bool TablesModel::insertRows (Report *report, int position, int rows, const QModelIndex &index) {

    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

        reports->append(report);

    endInsertRows();
    return true;
}

bool TablesModel::removeRows(int position, int rows, const QModelIndex &index) {

    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

        reports->removeAt(index.row());

    endRemoveRows();
    return true;
}

Qt::ItemFlags TablesModel::flags(const QModelIndex &index) const {

    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

Report* TablesModel::getReport(QModelIndex index) {

    return reports->at(index.row());
}
