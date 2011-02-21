#include "categorylabelsmodel.h"
#include "pmhdata.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

#include <translationutils/constanttranslations.h>

#include <QDebug>

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
    struct Language {
        Language(const QString &iso, const QString &label) :
                m_iso(iso), m_label(label)
        {
            lang = QLocale(iso).language();
        }

        void setIsoLanguage(const QString &iso)
        {
            m_iso = iso;
            lang = QLocale(iso).language();
        }

        QLocale::Language lang;
        QString m_iso;
        QString m_label;
    };
}
namespace PMH {
namespace Internal {
class CategoryLabelsModelPrivate
{
public:
    CategoryLabelsModelPrivate() : m_Cat(0) {}


public:
    PmhCategory *m_Cat;
    QList<Language> m_Labels;
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
    return d->m_Labels.count();
}

QVariant CategoryLabelsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= d->m_Labels.count())
        return QVariant();

    const Language &lang = d->m_Labels.at(index.row());

    if (role==Qt::EditRole) {
        if (index.column()==Lang) {
            return lang.m_iso;
        }
    }
    if (role==Qt::DisplayRole || role==Qt::EditRole) {
        switch (index.column()) {
        case Label : return lang.m_label;
        case Lang :
            {
                if (lang.m_iso==Trans::Constants::ALL_LANGUAGE) {
                    return tkTr(Trans::Constants::ALL_LANGUAGE_TEXT);
                }
                return qApp->translate("QLocale", qPrintable(QLocale::languageToString(lang.lang)));
            }
        }
    } else if (role==Qt::DecorationRole && index.column()==Lang) {
        if (lang.lang==QLocale::C) {
            return theme()->icon("flags/XX.png");
        } else {
            QLocale loc(lang.lang);
            return theme()->icon("flags/" + loc.name().right(2) + ".png");
        }
    }
    return QVariant();
}

bool CategoryLabelsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if (index.row() >= d->m_Labels.count())
        return false;
    if (role!=Qt::EditRole)
        return false;

    Language &lang = d->m_Labels[index.row()];
    switch (index.column()) {
    case Label : lang.m_label = value.toString();
    case Lang : lang.setIsoLanguage(value.toString());
    }

    Q_EMIT dataChanged(index, index);
    return true;
}

Qt::ItemFlags CategoryLabelsModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool CategoryLabelsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    int max = row+count;
    beginRemoveRows(parent, row, max-1);
    for(int i=0; i < count; ++i) {
        QString lang = index(row, Lang, parent).data(Qt::EditRole).toString();
        for(int i = 0; i < d->m_Labels.count(); ++i) {
            if (d->m_Labels.at(i).m_iso == lang) {
                d->m_Labels.removeAt(i);
                break;
            }
        }
    }
    endRemoveRows();
    return true;
}

bool CategoryLabelsModel::insertRows(int row, int count, const QModelIndex &parent)
{
    int max = row+count;
    beginInsertRows(parent, row, max-1);

    QString actual = QLocale().name().left(2);
    QStringList languages;
    for(int i = 0; i < d->m_Labels.count(); ++i) {
        languages.append(d->m_Labels.at(i).m_iso);
    }

    for(int i=0; i < count; ++i) {
        if (!languages.contains(actual)) {
            d->m_Labels.append(Language(actual, tkTr(Trans::Constants::FILENEW_TEXT)));
            languages.append(actual);
        } else if (!languages.contains(Trans::Constants::ALL_LANGUAGE)) {
            d->m_Labels.append(Language(Trans::Constants::ALL_LANGUAGE, tkTr(Trans::Constants::FILENEW_TEXT)));
            languages.append(Trans::Constants::ALL_LANGUAGE);
        } else {
            for(int j = 2; j < QLocale::LastLanguage; ++j) {
                QLocale::Language lang = static_cast<QLocale::Language>(j);
                QString l = QLocale(lang).name().left(2);
                if (!languages.contains(l)) {
                    d->m_Labels.append(Language(l, tkTr(Trans::Constants::FILENEW_TEXT)));
                    languages.append(l);
                    break;
                }
            }
        }
    }
    endInsertRows();
    return true;
}

// Data
bool CategoryLabelsModel::setPmhCategoryData(Internal::PmhCategory *cat)
{
    d->m_Cat = cat;
    d->m_Labels.clear();
    foreach(const QString &l, cat->allLanguagesForLabel()) {
        d->m_Labels.append(Language(l, cat->label(l)));
    }
    reset();
    return true;
}

Internal::PmhCategory *CategoryLabelsModel::pmhCategory() const
{
    return d->m_Cat;
}
