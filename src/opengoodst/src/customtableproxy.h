#ifndef CUSTOMTABLEPROXY_H
#define CUSTOMTABLEPROXY_H

#include <QSortFilterProxyModel>

class CustomTableProxy : public QSortFilterProxyModel
{
    bool sent;

public:
    CustomTableProxy(bool sent); //!The flag initializes the proxy class in sent table mode or not sent table mode.

    bool filterAcceptsColumn ( int source_column, const QModelIndex & source_parent ) const; //Reimplemented.
    bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;       //Reimplemented.
};

#endif // CUSTOMTABLEPROXY_H
