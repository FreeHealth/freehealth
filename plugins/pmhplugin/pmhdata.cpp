/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pmhdata.h"
#include "pmhepisodemodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>

#include <icdplugin/icdcollectionmodel.h>
#include <icdplugin/icdio.h>

#include <utils/log.h>

#include <QHash>
#include <QDate>

#include <QDebug>

using namespace PMH;
using namespace Internal;

static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline QString currentUserUuid() {return Core::ICore::instance()->user()->uuid();}


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
    d->m_Data.insert(PmhEpisodeData::DbOnly_IsValid, true);
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
    Category::CategoryItem *m_Category;
    PmhEpisodeModel *m_EpisodeModel;
};
}
}

/** \brief Create a new empty PmhData. */
PmhData::PmhData() :
        Category::ICategoryContentItem(), d(new PmhDataPrivate)
{
    // create default values
    populateWithCurrentData();
    d->m_Data.insert(ConfidenceIndex, 5);
    d->m_Data.insert(IsValid, true);
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
    d->m_Data.insert(PatientUid, patient()->uuid());
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

/** \brief Links the PmhData with a Category::CategoryItem. */
void PmhData::setCategory(Category::CategoryItem *cat)
{
    d->m_Category = cat;
}

Category::CategoryItem *PmhData::category() const
{
    return d->m_Category;
}
