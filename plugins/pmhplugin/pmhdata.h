/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Authors:                                                               *
 *       Eric MAEKER <eric.maeker@gmail.com>                               *
 *                                                                         *
 ***************************************************************************/
#pragma once

#include <translationutils/constanttranslations.h>

#include <categoryplugin/icategorycontentitem.h>

#include <QVariant>

/**
 * \file pmhdata.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 25 July 2012
*/

namespace ICD {
class IcdCollectionModel;
}

namespace PMH {
class PmhEpisodeModel;

namespace Internal {
class PmhData;
class PmhDataPrivate;
class PmhEpisodeDataPrivate;

class PmhEpisodeData {
public:
    enum DataRepresentation {
        Label = 0,
        DateStart,
        DateEnd,
        ConfidenceIndex,
        IcdCodeList,
        IcdLabelStringList,
        IcdLabelHtmlList,
        IcdXml,
        Comment,
        DbOnly_Id,
        DbOnly_IsValid,
        DbOnly_MasterId
    };

    PmhEpisodeData();
    ~PmhEpisodeData();

    bool setData(const int ref, const QVariant &value);
    QVariant data(const int ref) const;

    ICD::IcdCollectionModel *icdModel() const;

private:
    PmhEpisodeDataPrivate *d;
};


class PmhData : public Category::ICategoryContentItem
{
public:
    enum DataRepresentation {
        Uid = 0,
        UserOwner,
        PatientUid,
        Label,
        Type,
        State,
        IsValid,
        ConfidenceIndex,
        Comment,
        CategoryId,
        IsPrivate,
        DbOnly_MasterEpisodeId,
        DbOnly_MasterContactId,
        CreationDateTime
    };

    PmhData();
    ~PmhData();

    void populateWithCurrentData();
    bool setData(const int ref, const QVariant &value);
    QVariant data(const int ref) const;

    void addEpisode(PmhEpisodeData *episode);
    bool insertEpisode(int pos, PmhEpisodeData *episode);
    bool removeEpisode(PmhEpisodeData *episode);
    QList<PmhEpisodeData *> episodes() const;
    PmhEpisodeModel *episodeModel();


    // Category::ICategoryContentItem
    int categoryId() const {return data(CategoryId).toInt();}
    void setCategory(Category::CategoryItem *cat);
    Category::CategoryItem *category() const;

private:
    PmhDataPrivate *d;
};


} // End namespace Internal
} // End namespace PMH
