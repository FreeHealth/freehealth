#include "dailyschememodel.h"


namespace {
    struct DailySchemeStruct {
    };
}

namespace DrugsDB {
namespace Internal {

class DailyScheme {
public:
    DailyScheme();

    /** \brief Use Trans::DailyScheme enum for \e dayRef */
    void setValue(const int dayRef, const double value)
    {
        m_DayRef_Value.insert(dayRef,value);
    }

    double value(const int dayRef) const
    {
        m_DayRef_Value.value(dayRef, 0);
    }

private:
    QHash<int, double> m_DayRef_Value;
};


class DailySchemeModelPrivate
{
public:
    DailySchemeModelPrivate() : m_IsScored(false), m_Min(0), m_Max(1) {}

    QHash<int, double> m_DailySchemes;
    bool m_IsScored;
    double m_Min, m_Max;
};

} // end namespace Internal
} // end namespace DrugsDB


using namespace DrugsDB;

DailySchemeModel::DailySchemeModel(QObject * parent, const QString &serializedContent) :
        QAbstractTableModel(parent), d(0)
{
    d = new Internal::DailySchemeModelPrivate();
}

DailySchemeModel::~DailySchemeModel()
{
    if (d) {
        delete d;
        d = 0;
    }
}

QString DailySchemeModel::serializedContent() const
{
    return QString();
}
void DailySchemeModel::setSerializedContent(const QString &content)
{}

void DailySchemeModel::setScored(bool isScored)
{
    d->m_IsScored = isScored;
}

void DailySchemeModel::setDayRange(double min, double max)
{
    d->m_Min = min;
    d->m_Max = max;
    /** \todo recalculate daily scheme and manage errors ? */
}

Qt::ItemFlags DailySchemeModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

bool DailySchemeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if ((role==Qt::EditRole) || (role==Qt::DisplayRole)) {
        if (index.column()==Value) {
            d->m_DailySchemes[index.row()] = value.toDouble();
            return true;
        }
    }
    return false;
}

QVariant DailySchemeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ((role==Qt::EditRole) || (role==Qt::DisplayRole)) {
        if (index.column()==Value)
            return d->m_DailySchemes.value(index.row(), 0);
        else if (index.column()==DayReference)
            return Trans::ConstantTranslations::dailyScheme(index.row());
    }
    return QVariant();
}

