/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>                     *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "availablemovementmodel.h"
#include "constants.h"
#include "accountbase.h"

#include <translationutils/constanttranslations.h>

#include <QAbstractTableModel>
#include <QSqlTableModel>

using namespace AccountDB;

using namespace Trans::ConstantTranslations;


namespace AccountDB {
namespace Internal {
class AvailableMovementModelPrivate
{
public:
    AvailableMovementModelPrivate(AvailableMovementModel *parent) : m_SqlTable(0), q(parent)
    {
        m_SqlTable = new QSqlTableModel(q,QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
    }

    ~AvailableMovementModelPrivate () {}

public:
    QSqlTableModel *m_SqlTable;


private:
    AvailableMovementModel *q;
};
}
}



AvailableMovementModel::AvailableMovementModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::AvailableMovementModelPrivate(this))
{
    d->m_SqlTable->setTable(AccountBase::instance()->table(Constants::Table_AvailableMovement));
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);
    //    d->m_SqlTable->setFilter( user );
    d->m_SqlTable->select();
}

AvailableMovementModel::~AvailableMovementModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int AvailableMovementModel::rowCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->rowCount(parent);
}

int AvailableMovementModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}


QVariant AvailableMovementModel::data(const QModelIndex &index, int role) const
{
    return d->m_SqlTable->data(index, role);
}

bool AvailableMovementModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return d->m_SqlTable->setData(index, value, role);
}


QVariant AvailableMovementModel::headerData(int section, Qt::Orientation orientation, int role) const
{return QVariant();}

bool AvailableMovementModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool b = d->m_SqlTable->insertRows(row, count, parent);
    
    return b;
   }

bool AvailableMovementModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool b = d->m_SqlTable->removeRows(row, count, parent);
    
    return b;

}

bool AvailableMovementModel::isDirty() const
{
    return true;
}

void AvailableMovementModel::setFilter(const QString & filter){
    d->m_SqlTable->setFilter(filter);
    d->m_SqlTable->select();
}

bool AvailableMovementModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        return true;
    }
    return false;
}

QSqlError AvailableMovementModel::lastError(){
    return d->m_SqlTable->lastError();
}

