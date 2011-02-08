/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "pmhdata.h"
#include "pmhepisodemodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>

#include <icdplugin/icdcollectionmodel.h>
#include <icdplugin/icdio.h>

#include <QHash>
#include <QDate>

#include <QDebug>

using namespace PMH;
using namespace Internal;

static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline QString currentUserUuid() {return Core::ICore::instance()->user()->value(Core::IUser::Uuid).toString();}

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
    if (number < d->m_Children.count()) {
        return d->m_Children.at(number);
    }
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

void PmhCategory::addPhmData(PmhData *data)
{
    d->m_PmhData << data;
}

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
    return true;
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
    PmhEpisodeDataPrivate() : m_IcdModel(0) {}
    ~PmhEpisodeDataPrivate()
    {
        if (m_IcdModel)
            delete m_IcdModel;
        m_IcdModel = 0;
    }

public:
    QHash<int, QVariant> m_Data;
    ICD::IcdCollectionModel *m_IcdModel;
};
}
}

PmhEpisodeData::PmhEpisodeData() :
        d(new PmhEpisodeDataPrivate)
{
    // Default start date is currentDate
    d->m_Data.insert(PmhEpisodeData::DateStart, QDate::currentDate());
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
    if (ref == IcdXml) {
        if (!d->m_IcdModel)
            d->m_IcdModel = new ICD::IcdCollectionModel;
        ICD::IcdIO io;
        io.icdCollectionFromXml(d->m_IcdModel, value.toString());
    }
    d->m_Data.insert(ref, value);
    return true;
}

QVariant PmhEpisodeData::data(const int ref) const
{
    if (ref == IcdXml) {
        if (!d->m_IcdModel)
            return QVariant();
        ICD::IcdIO io;
        return io.icdCollectionToXml(d->m_IcdModel);
    } else if (ref==IcdCodeList) {
        if (d->m_IcdModel)
            return d->m_IcdModel->includedCodesWithDaget().join(";");
        return QVariant();
    } else if (ref==IcdLabelHtmlList) {
        if (d->m_IcdModel) {
            return d->m_IcdModel->includedLabelsToHtml();
        }
        return QVariant();
    } else if (ref==IcdLabelStringList) {
        if (d->m_IcdModel) {
            return d->m_IcdModel->includedLabels();
        }
        return QVariant();
    }
    return d->m_Data.value(ref);
}

ICD::IcdCollectionModel *PmhEpisodeData::icdModel() const
{
    return d->m_IcdModel;
}




namespace PMH {
namespace Internal {
class PmhDataPrivate
{
public:
    PmhDataPrivate() : m_Category(0), m_EpisodeModel(0){}
    ~PmhDataPrivate()
    {
        if (m_EpisodeModel) {
            delete m_EpisodeModel;
        }
        m_EpisodeModel = 0;

        qDeleteAll(m_Episodes);
        m_Episodes.clear();
    }

public:
    QHash<int, QVariant> m_Data;
    QList<PmhEpisodeData *> m_Episodes;
    PmhCategory *m_Category;
    PmhEpisodeModel *m_EpisodeModel;
};
}
}

/** \brief Create a new empty PmhData. */
PmhData::PmhData() :
        d(new PmhDataPrivate)
{
    // create default values
    populateWithCurrentData();
    d->m_Data.insert(ConfidenceIndex, 5);
}

PmhData::~PmhData()
{
    if (d) {
        delete d;
    }
    d = 0;
}

/** \brief Populate the PmhData with the current User Uid and the current Patient Uid. */
void PmhData::populateWithCurrentData()
{
    d->m_Data.insert(PatientUid, patient()->data(Core::IPatient::Uid));
    d->m_Data.insert(UserOwner, currentUserUuid());
}

bool PmhData::setData(const int ref, const QVariant &value)
{
    d->m_Data.insert(ref, value);
    // if Uid -> update all episodes
    if (ref == Uid) {
        foreach(PmhEpisodeData *ep, d->m_Episodes) {
            ep->setData(PmhEpisodeData::DbOnly_MasterId, value);
        }
    }
    return true;
}

QVariant PmhData::data(const int ref) const
{
    return d->m_Data.value(ref);
}

/** \brief Add a PmhEpisodeData \e episode into the PmhData. */
void PmhData::addEpisode(PmhEpisodeData *episode)
{
    Q_ASSERT(episode);
    if (!episode)
        return;
    episode->setData(PmhEpisodeData::DbOnly_MasterId, d->m_Data.value(PmhData::Uid));
    d->m_Episodes.append(episode);
}

/** \brief Insert a PmhEpisodeData \e episode into the PmhData at the specified position \e pos. */
bool PmhData::insertEpisode(int pos, PmhEpisodeData *episode)
{
    Q_ASSERT(episode);
    if (!episode)
        return false;
    episode->setData(PmhEpisodeData::DbOnly_MasterId, d->m_Data.value(PmhData::Uid));
    if (pos >= d->m_Episodes.count()) {
        d->m_Episodes.append(episode);
        return true;
    }
    d->m_Episodes.insert(pos, episode);
    return true;
}

/** \brief Remove PmhEpisodeData from the PmhData **and** delete the pointer. */
bool PmhData::removeEpisode(PmhEpisodeData *episode)
{
    Q_ASSERT(episode);
    if (!episode)
        return false;
    if (!d->m_Episodes.contains(episode))
        return false;
    d->m_Episodes.removeAll(episode);
    delete episode;
    episode = 0;
    return true;
}

/** \brief Returns the full ordered list of PmhEpisodeData of the PmhData. */
QList<PmhEpisodeData *> PmhData::episodes() const
{
    return d->m_Episodes;
}

/**
  \brief Creates and returns the PmhEpisodeModel populated with the actual data of the PmhData.
  Model should not be deleted.
*/
PmhEpisodeModel *PmhData::episodeModel()
{
    if (!d->m_EpisodeModel)
        d->m_EpisodeModel = new PmhEpisodeModel(this);
    return d->m_EpisodeModel;
}

/** \brief Links the PmhData with a PmhCategory. */
void PmhData::setCategory(PmhCategory *cat)
{
    d->m_Category = cat;
}
