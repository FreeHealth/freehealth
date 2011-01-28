#ifndef PMHDATA_H
#define PMHDATA_H

#include <translationutils/constanttranslations.h>

#include <QVariant>

namespace PMH {
namespace Internal {
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

//    void addPhmData(PmhData *data);
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
        DateStart,
        DateEnd,
        ConfidenceIndex,
        IcdXml,
        DbOnly_Id,
        DbOnly_MasterId,
    };

    PmhEpisodeData();
    ~PmhEpisodeData();

    bool setData(const int ref, const QVariant &value);
    QVariant data(const int ref) const;

private:
    PmhEpisodeDataPrivate *d;
};


class PmhData
{
public:
    enum DataRepresentation {
        Uid = 0,
        UserOwner,
        Label,
        Type,
        State,
        CategoryId,
        DbOnly_MasterEpisodeId,
        DbOnly_MasterContactId,
    };

    PmhData();
    ~PmhData();

    bool setData(const int ref, const QVariant &value);
    QVariant data(const int ref) const;

    void addEpisode(PmhEpisodeData *episode);
    void setCategory(PmhCategory *cat);

    // Code beautifyers
    int categoryId() const {return data(CategoryId).toInt();}

private:
    PmhDataPrivate *d;
};


} // End namespace Internal
} // End namespace PMH

#endif // PMHDATA_H
