#include "globaldrugsmodel.h"

#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/constants.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants.h>

#include <QList>
#include <QColor>

#include <QDebug>

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}

namespace DrugsDB {
namespace Internal {

class GlobalDrugsModelPrivate
{
public:
    GlobalDrugsModelPrivate::GlobalDrugsModelPrivate()
    {}

    void updateCachedAvailableDosage()
    {
        m_CachedAvailableDosageForCIS.clear();
        m_CachedAvailableDosageForCIS = drugsBase()->getAllCISThatHaveRecordedDosages();
    }

    bool CISHasRecordedDosage(const int CIS)
    {
        return m_CachedAvailableDosageForCIS.contains(CIS);
    }

private:
    QList<int> m_CachedAvailableDosageForCIS;

};
}  // End Internal
}  // End DrugsDB

using namespace DrugsDB;

DrugsDB::GlobalDrugsModel *DrugsDB::GlobalDrugsModel::m_Instance = 0;
DrugsDB::GlobalDrugsModel *DrugsDB::GlobalDrugsModel::instance(QObject *parent)
{
    if (!m_Instance)
        m_Instance = new DrugsDB::GlobalDrugsModel(parent);
    return m_Instance;
}

GlobalDrugsModel::GlobalDrugsModel(QObject *parent) :
        QSqlTableModel(parent, QSqlDatabase::database(Constants::DRUGS_DATABASE_NAME)),
        d(0)
{
    d = new Internal::GlobalDrugsModelPrivate();
    d->updateCachedAvailableDosage();
    setTable(drugsBase()->table(Constants::Table_CIS));
    setEditStrategy( QSqlTableModel::OnManualSubmit );
    QHashWhere where;
    where.insert(Constants::CIS_COMMERCIALISATION, "='O'");
    setFilter(drugsBase()->getWhereClause(Constants::Table_CIS, where));
    select();
}

QVariant GlobalDrugsModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
        return QVariant();

    if (role == Qt::BackgroundRole) {
        if (item.column() == Constants::CIS_DENOMINATION) {
            if (settings()->value(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES).toBool()) {
                QModelIndex cis = index(item.row(), Constants::CIS_CIS);
                if (d->CISHasRecordedDosage(cis.data().toInt())) {
                    QColor c = QColor(settings()->value(Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR).toString());
                    c.setAlpha(125);
                    return c;
                }
            }
        }
    }
    return QSqlTableModel::data(item,role);
}

Qt::ItemFlags GlobalDrugsModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void GlobalDrugsModel::updateCachedAvailableDosage()
{
    d->updateCachedAvailableDosage();
}
