#include "workingplacesmodel.h"
#include "constants.h"
#include "accountbase.h"

#include <translationutils/constanttranslations.h>

#include <QAbstractTableModel>
#include <QSqlTableModel>

using namespace AccountDB;

using namespace Trans::ConstantTranslations;


namespace AccountDB {
namespace Internal {
class WorkingPlacesModelPrivate
{
public:
    WorkingPlacesModelPrivate(WorkingPlacesModel *parent) : m_SqlTable(0), q(parent)
    {
        m_SqlTable = new QSqlTableModel(q);
    }

    ~WorkingPlacesModelPrivate () {}

public:
    QSqlTableModel *m_SqlTable;


private:
    WorkingPlacesModel *q;
};
}
}



WorkingPlacesModel::WorkingPlacesModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::WorkingPlacesModelPrivate(this))
{
    d->m_SqlTable->setTable(AccountBase::instance()->table(Constants::Table_Sites));
    //    d->m_SqlTable->setEditStrategy();
    //    d->m_SqlTable->setFilter( user );
    d->m_SqlTable->select();
}

WorkingPlacesModel::~WorkingPlacesModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int WorkingPlacesModel::rowCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->rowCount(parent);
}

int WorkingPlacesModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}


QVariant WorkingPlacesModel::data(const QModelIndex &index, int role) const
{
    return d->m_SqlTable->data(index, role);
}

bool WorkingPlacesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return d->m_SqlTable->setData(index, value, role);
}


QVariant WorkingPlacesModel::headerData(int section, Qt::Orientation orientation, int role) const
{}

bool WorkingPlacesModel::insertRows(int row, int count, const QModelIndex &parent)
{}

bool WorkingPlacesModel::removeRows(int row, int count, const QModelIndex &parent)
{}

bool WorkingPlacesModel::isDirty() const
{
    return true;
}

