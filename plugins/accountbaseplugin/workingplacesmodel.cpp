#include "workingplacesmodel.h"
#include "constants.h"
#include "accountbase.h"

#include <translationutils/constanttranslations.h>

#include <QAbstractTableModel>
#include <QSqlTableModel>

#include <QDebug>



using namespace AccountDB;

using namespace Trans::ConstantTranslations;


namespace AccountDB {
namespace Internal {
class WorkingPlacesModelPrivate
{
public:
    WorkingPlacesModelPrivate(WorkingPlacesModel *parent) : m_SqlTable(0), q(parent)
    {
        m_SqlTable = new QSqlTableModel(q,QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
        m_SqlTable->setTable(AccountBase::instance()->table(Constants::Table_Sites));
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
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);
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
    int rows = 0;
    d->m_SqlTable->setFilter("");
    d->m_SqlTable->select();
    rows = d->m_SqlTable->rowCount(parent);
    return rows;
}

int WorkingPlacesModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}


QVariant WorkingPlacesModel::data(const QModelIndex &index, int role) const
{
    QVariant data ;
    if(!index.isValid()){
       qWarning() << __FILE__ << QString::number(__LINE__) << "index is not valid" ;
       data = QVariant();
       }
    if(role == Qt::DisplayRole){
        data = d->m_SqlTable->data(index, Qt::DisplayRole);
        }	
    if (role == Qt::EditRole)
    {
    	data = d->m_SqlTable->data(index, Qt::EditRole);
        }
    return data;
}

bool WorkingPlacesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return d->m_SqlTable->setData(index, value, role);
}


QVariant WorkingPlacesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

bool WorkingPlacesModel::insertRows(int row, int count, const QModelIndex &parent)
{
      bool ret = d->m_SqlTable->insertRows(row, count, parent);
      return ret;
}

bool WorkingPlacesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return d->m_SqlTable->removeRows(row, count, parent);
}

bool WorkingPlacesModel::isDirty() const
{
    return true;
}

bool WorkingPlacesModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        return true;
    }
    return false;
}

void  WorkingPlacesModel::revert()
{
    d->m_SqlTable->revert();
}

QSqlError WorkingPlacesModel::lastError(){
    return d->m_SqlTable->lastError();
}

void WorkingPlacesModel::setFilter(const QString & filter){
    d->m_SqlTable->setFilter(filter);
    d->m_SqlTable->select();
}

