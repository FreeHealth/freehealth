#include "pmhdata.h"

#include <QHash>

#include <QDebug>

using namespace PMH::Internal;

namespace PMH {
namespace Internal {
class PmhCategoryPrivate
{
public:
    PmhCategoryPrivate() : m_Parent(0) {}

    ~PmhCategoryPrivate()
    {
        qDeleteAll(m_Children);
        m_Children.clear();
    }

public:
    QHash<int, QVariant> m_Data;
    QHash<QString, QString> m_Labels; //K=lang, V=label
    PmhCategory *m_Parent;
    QList<PmhCategory *> m_Children;
    QList<PmhData *> m_PmhData;
};
}
}

PmhCategory::PmhCategory() :
        d(new PmhCategoryPrivate)
{
}

PmhCategory::~PmhCategory()
{
    if (d) {
        delete d;
    }
    d = 0;
}

void PmhCategory::setParent(PmhCategory *parent)
{
    d->m_Parent = parent;
}

PmhCategory *PmhCategory::parent() const
{
    return d->m_Parent;
}

void PmhCategory::addChild(PmhCategory *child)
{
    d->m_Children.append(child);
}

PmhCategory *PmhCategory::child(int number) const
{
    if (number < d->m_Children.count())
        return d->m_Children.at(number);
    return 0;
}

QList<PmhCategory *> PmhCategory::children() const
{
    return d->m_Children;
}

int PmhCategory::childCount() const
{
    return d->m_Children.count();
}

int PmhCategory::childNumber() const
{
    if (d->m_Parent)
        return d->m_Parent->children().indexOf(const_cast<PmhCategory*>(this));
    return 0;
}

bool PmhCategory::setData(const int ref, const QVariant &value)
{
    d->m_Data.insert(ref, value);
    return true;
}

QVariant PmhCategory::data(const int ref) const
{
    return d->m_Data.value(ref);
}

bool PmhCategory::setLabel(const QString &label, const QString &lang)
{
    d->m_Labels.insert(lang, label);
    return true;
}

QString PmhCategory::label(const QString &lang) const
{
    return d->m_Labels.value(lang);
}

//void PmhCategory::addPhmData(PmhData *data)
//{
//    d->m_PmhData << data;
//}

//QList<PmhData *> PmhCategory::phmDataChildren() const
//{
//    return d->m_PmhData;
//}

//void PmhCategory::phmDataCount() const
//{
//    return d->m_PmhData.count();
//}

bool PmhCategory::sortChildren()
{
    qSort(d->m_Children.begin(), d->m_Children.end(), PmhCategory::lessThan);
}

bool PmhCategory::lessThan(const PmhCategory *c1, const PmhCategory *c2)
{
    return c1->sortId() < c2->sortId();
}


namespace PMH {
namespace Internal {
class PmhEpisodeDataPrivate
{
public:
    PmhEpisodeDataPrivate() {}
    ~PmhEpisodeDataPrivate() {}

public:
    QHash<int, QVariant> m_Data;
};
}
}

PmhEpisodeData::PmhEpisodeData() :
        d(new PmhEpisodeDataPrivate)
{
}

PmhEpisodeData::~PmhEpisodeData()
{
    if (d) {
        delete d;
    }
    d = 0;
}

bool PmhEpisodeData::setData(const int ref, const QVariant &value)
{
    d->m_Data.insert(ref, value);
    return true;
}

QVariant PmhEpisodeData::data(const int ref) const
{
    return d->m_Data.value(ref);
}



namespace PMH {
namespace Internal {
class PmhDataPrivate
{
public:
    PmhDataPrivate() {}
    ~PmhDataPrivate()
    {
        qDeleteAll(m_Episodes);
        m_Episodes.clear();
    }

public:
    QHash<int, QVariant> m_Data;
    QList<PmhEpisodeData *> m_Episodes;
    PmhCategory *m_Category;
};
}
}

PmhData::PmhData() :
        d(new PmhDataPrivate)
{
}

PmhData::~PmhData()
{
    if (d) {
        delete d;
    }
    d = 0;
}

bool PmhData::setData(const int ref, const QVariant &value)
{
    d->m_Data.insert(ref, value);
    return true;
}

QVariant PmhData::data(const int ref) const
{
    return d->m_Data.value(ref);
}

void PmhData::addEpisode(PmhEpisodeData *episode)
{
    d->m_Episodes.append(episode);
}

void PmhData::setCategory(PmhCategory *cat)
{
    d->m_Category = cat;
}
