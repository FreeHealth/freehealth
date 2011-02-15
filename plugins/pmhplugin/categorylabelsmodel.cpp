#include "categorylabelsmodel.h"
#include "pmhdata.h"

#include <QDebug>


using namespace PMH;
using namespace Internal;

namespace PMH {
namespace Internal {
class CategoryLabelsModelPrivate
{
public:
    CategoryLabelsModelPrivate() : m_Cat(0) {}


public:
    PmhCategory *m_Cat;
};
}  // End namespace Internal
}  // End namespace PMH



CategoryLabelsModel::CategoryLabelsModel(QObject *parent) :
    QAbstractTableModel(parent), d(new CategoryLabelsModelPrivate)
{
}

CategoryLabelsModel::~CategoryLabelsModel()
{
    if (d)
        delete d;
    d = 0;
}

int CategoryLabelsModel::rowCount(const QModelIndex &parent) const
{
    if (!d->m_Cat)
        return 0;
    return d->m_Cat->allLanguagesForLabel().count();
}

QVariant CategoryLabelsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= d->m_Cat->allLanguagesForLabel().count())
        return QVariant();

    if (role==Qt::DisplayRole || role==Qt::EditRole) {
        const QString &lang = d->m_Cat->allLanguagesForLabel().at(index.row());
        switch (index.column()) {
        case Label : return d->m_Cat->label(lang);
        case Lang : return lang;
        }
    }
    return QVariant();
}

bool CategoryLabelsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if (index.row() >= d->m_Cat->allLanguagesForLabel().count())
        return false;
    const QString &lang = d->m_Cat->allLanguagesForLabel().at(index.row());
    switch (index.column()) {
    case Label : d->m_Cat->setLabel(value.toString(), lang);
    case Lang : d->m_Cat->allLanguagesForLabel()[index.row()] = value.toString();
    }
    Q_EMIT dataChanged(index, index);
    return true;
}

Qt::ItemFlags CategoryLabelsModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

// Data
bool CategoryLabelsModel::setPmhCategoryData(Internal::PmhCategory *cat)
{
    d->m_Cat = cat;
    reset();
    return true;
}

Internal::PmhCategory *CategoryLabelsModel::pmhCategory() const
{
    return d->m_Cat;
}
