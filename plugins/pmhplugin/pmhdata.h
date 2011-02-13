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
#ifndef PMHDATA_H
#define PMHDATA_H

#include <translationutils/constanttranslations.h>

#include <QVariant>

namespace ICD {
class IcdCollectionModel;
}

namespace PMH {
class PmhEpisodeModel;

namespace Internal {
class PmhData;
class PmhDataPrivate;
class PmhEpisodeDataPrivate;
class PmhCategoryPrivate;


//CATEGORY_ID = 0, CATEGORY_PARENT, CATEGORY_LABEL_ID, CATEGORY_ISRISKFACTOR,
//CATEGORY_ISCHONICDISEASE,CATEGORY_THEMEDICON,

//CATEGORYLABEL_ID = 0, CATEGORYLABEL_LANG, CATEGORYLABEL_VALUE,

class PmhCategory {
public:
    enum DataRepresentation {
        DbOnly_Id = 0,
        DbOnly_LabelId,
        DbOnly_ParentId,
        ThemedIcon,
        SortId
    };

    PmhCategory();
    ~PmhCategory();

    void setParent(PmhCategory *parent);
    PmhCategory *parent() const;
    void addChild(PmhCategory *child);
    PmhCategory *child(int number) const;
    QList<PmhCategory *> children() const;
    int childCount() const;
    int childNumber() const;

    bool setData(const int ref, const QVariant &value);
    QVariant data(const int ref) const;

    bool setLabel(const QString &label, const QString &lang = Trans::Constants::ALL_LANGUAGE);
    QString label(const QString &lang = Trans::Constants::ALL_LANGUAGE) const;
    QStringList allLanguagesForLabel() const;

    void addPhmData(PmhData *data);
//    QList<PmhData *> phmDataChildren() const;
//    void phmDataCount() const;

    bool sortChildren();
    static bool lessThan(const PmhCategory *c1, const PmhCategory *c2);

    // code beautifying
    int id() const {return data(DbOnly_Id).toInt();}
    int parentId() const {return data(DbOnly_ParentId).toInt();}
    int sortId() const  {return data(SortId).toInt();}
    QString iconName() const {return data(ThemedIcon).toString();}

private:
    PmhCategoryPrivate *d;
};


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
        DbOnly_MasterId,
    };

    PmhEpisodeData();
    ~PmhEpisodeData();

    bool setData(const int ref, const QVariant &value);
    QVariant data(const int ref) const;

    ICD::IcdCollectionModel *icdModel() const;

private:
    PmhEpisodeDataPrivate *d;
};


class PmhData
{
public:
    enum DataRepresentation {
        Uid = 0,
        UserOwner,
        PatientUid,
        Label,
        Type,
        State,
        ConfidenceIndex,
        Comment,
        CategoryId,
        DbOnly_MasterEpisodeId,
        DbOnly_MasterContactId,
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

    void setCategory(PmhCategory *cat);

    // Code beautifyers
    int categoryId() const {return data(CategoryId).toInt();}

private:
    PmhDataPrivate *d;
};


} // End namespace Internal
} // End namespace PMH

#endif // PMHDATA_H
