/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "categorylabelsmodel.h"
#include "categoryitem.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

#include <translationutils/constanttranslations.h>

#include <QDebug>

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

using namespace Category;
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

        void setLanguage(QLocale::Language l)
        {
            this->lang = l;
            m_iso = QLocale(l).name().left(2);
        }

        QLocale::Language lang;
        QString m_iso;
        QString m_label;
    };
}
namespace Category {
namespace Internal {
class CategoryLabelsModelPrivate
{
public:
    CategoryLabelsModelPrivate() : m_Cat(0) {}


public:
    CategoryItem *m_Cat;
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
    // prevent trees
    if (parent.isValid()) return 0;

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
            return lang.lang;
        }
    }
    if (role==Qt::DisplayRole || role==Qt::EditRole) {
        switch (index.column()) {
        case Label : return lang.m_label;
        case Lang :
            {
                if (lang.m_iso==Trans::Constants::ALL_LANGUAGE || lang.lang==QLocale::C) {
                    if (d->m_Labels.count() > 1)
                        return tkTr(Trans::Constants::ALL_OTHER_LANGUAGES_TEXT);
                    else
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
    } else if (role==Qt::ToolTipRole) {
        QString language;
        if (lang.m_iso==Trans::Constants::ALL_LANGUAGE || lang.lang==QLocale::C) {
            if (d->m_Labels.count() > 1)
                language = tkTr(Trans::Constants::ALL_OTHER_LANGUAGES_TEXT);
            else
                language = tkTr(Trans::Constants::ALL_LANGUAGE_TEXT);
        } else {
            qApp->translate("QLocale", qPrintable(QLocale::languageToString(lang.lang)));
        }
        // FIXME: use tkTr(Trans::Constants::_1_COLON_2) "%1: %2" translation
        return tkTr(Trans::Constants::_1_COLON_2).arg(language).arg(lang.m_label);
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
    case Label : lang.m_label = value.toString(); break;
    case Lang : lang.setLanguage(static_cast<QLocale::Language>(value.toInt())); break;
    }
    d->m_Cat->setLabel(lang.m_label, lang.m_iso);
    Q_EMIT dataChanged(index, index);
    Q_EMIT labelChanged(d->m_Cat);
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
        d->m_Labels.removeAt(row);
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
            d->m_Labels.insert(row+i, Language(actual, tkTr(Trans::Constants::FILENEW_TEXT).remove("&")));
            languages.append(actual);
        } else if (!languages.contains(Trans::Constants::ALL_LANGUAGE)) {
            d->m_Labels.insert(row+i, Language(Trans::Constants::ALL_LANGUAGE, tkTr(Trans::Constants::FILENEW_TEXT).remove("&")));
            languages.append(Trans::Constants::ALL_LANGUAGE);
        } else {
            for(int j = 2; j < QLocale::LastLanguage; ++j) {
                QLocale::Language lang = static_cast<QLocale::Language>(j);
                QString l = QLocale(lang).name().left(2);
                if (!languages.contains(l)) {
                    d->m_Labels.insert(row+i, Language(l, tkTr(Trans::Constants::FILENEW_TEXT).remove("&")));
                    languages.append(l);
                    break;
                }
            }
        }
    }
    endInsertRows();
    return true;
}

bool CategoryLabelsModel::submit()
{
    d->m_Cat->clearLabels();
    foreach(const Language &lang, d->m_Labels) {
        d->m_Cat->setLabel(lang.m_label, lang.m_iso);
    }
    return true;
}

// Data
bool CategoryLabelsModel::setCategoryItem(CategoryItem *cat)
{
    beginResetModel();
    d->m_Cat = cat;
    d->m_Labels.clear();
    foreach(const QString &l, cat->allLanguagesForLabel()) {
        d->m_Labels.append(Language(l, cat->label(l)));
    }
    endResetModel();
    return true;
}

CategoryItem *CategoryLabelsModel::categoryItem() const
{
    return d->m_Cat;
}
