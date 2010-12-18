#include "thesaurusmodel.h"
#include "constants.h"
#include "accountbase.h"

#include <translationutils/constanttranslations.h>

#include <QAbstractTableModel>
#include <QSqlTableModel>

using namespace AccountDB;

using namespace Trans::ConstantTranslations;


namespace AccountDB {
namespace Internal {
class ThesaurusModelPrivate
{
public:
    ThesaurusModelPrivate(ThesaurusModel *parent) : m_SqlTable(0), q(parent)
    {
        m_SqlTable = new QSqlTableModel(q);
    }

    ~ThesaurusModelPrivate () {}

public:
    QSqlTableModel *m_SqlTable;


private:
    ThesaurusModel *q;
};
}
}



ThesaurusModel::ThesaurusModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::ThesaurusModelPrivate(this))
{
    d->m_SqlTable->setTable(AccountBase::instance()->table(Constants::Table_Sites));
    //    d->m_SqlTable->setEditStrategy();
    //    d->m_SqlTable->setFilter( user );
    d->m_SqlTable->select();
}

ThesaurusModel::~ThesaurusModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int ThesaurusModel::rowCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->rowCount(parent);
}

int ThesaurusModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}


QVariant ThesaurusModel::data(const QModelIndex &index, int role) const
{
    return d->m_SqlTable->data(index, role);
}

bool ThesaurusModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return d->m_SqlTable->setData(index, value, role);
}


QVariant ThesaurusModel::headerData(int section, Qt::Orientation orientation, int role) const
{}

bool ThesaurusModel::insertRows(int row, int count, const QModelIndex &parent)
{}

bool ThesaurusModel::removeRows(int row, int count, const QModelIndex &parent)
{}

bool ThesaurusModel::isDirty() const
{
    return true;
}

