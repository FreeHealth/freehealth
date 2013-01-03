/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MULTILINGUALCLASSTEMPLATE_H
#define MULTILINGUALCLASSTEMPLATE_H

#include <translationutils/constants.h>

#include <QString>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QHash>

/**
 * \file multilingualclasstemplate.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 15 Mar 2011
*/

namespace Trans {
/**
 * \brief This template can be used to manage a multilingual data class.
 * It manages a default set for all language, and one for each languages.
 *
 * \code
 * // Declare a class
 * class MyMultiLingualClass : public Trans::MultiLingualClass<OneDataPerLangClass>
 * {
 * public:
 *   MyMultiLingualClass() {}
 *   ~MyMultiLingualClass() {}
 *
 *   QString categoryForTreeWiget() const {return QString("My Text to show for the debug dialog");}
 * };
 *
 * // Then is the MyMultiLingualClass find your data by lang name
 * // lang should be 2 char locale
 * if (d->hasLanguage(lang))
 *   s = d->getLanguage(lang);
 * else
 *   s = d->createLanguage(lang);
 * \endcode
 */
template <typename T>
class MultiLingualClass
{
public:
    MultiLingualClass() {}
    virtual ~MultiLingualClass()
    {
        m_Hash_T_Lang.clear();
    }

    /** Use for the debugging treewidget construction */
    QString categoryForTreeWiget() const {return QString();}

    /** Check if the \e lang already exists */
    bool hasLanguage(const QString &lang)
    {
        return m_Hash_T_Lang.contains(lang.left(2));
    }

    /** Return all included language. */
    QStringList languages() const
    {
        return m_Hash_T_Lang.uniqueKeys();
    }

    /**
        Return a pointer to the data class for one unique \e language,
        or if it does not exist for the default common language.
        If \e language does not exists return 0.
     */
    T *getLanguage(const QString &language)
    {
        QString tmp = language.left(2);
        if (m_Hash_T_Lang.contains(tmp)) {
            return &m_Hash_T_Lang[tmp];
        } else {
            if (m_Hash_T_Lang.contains(Constants::ALL_LANGUAGE))
                return &m_Hash_T_Lang[Constants::ALL_LANGUAGE];
        }
        return 0;
    }


    /** Return a pointer to the data class for one unique \e language. If \e language doesnot exists it is created. */
    T *createLanguage(const QString &lang)
    {
        QString tmp = lang.left(2);
        if (m_Hash_T_Lang.contains(tmp)) {
            return &m_Hash_T_Lang[tmp];
        } else {
            T s;
            m_Hash_T_Lang.insert(tmp, s);
            return &m_Hash_T_Lang[tmp];
        }
        return 0;
    }

    /** Clear all recorded data and languages. */
    void clear()
    {
        m_Hash_T_Lang.clear();
    }

    /** For debugging purpose */
    void toTreeWidget(QTreeWidgetItem *tree) const
    {
        QTreeWidgetItem *l = 0;
        QFont bold;
        bold.setBold(true);
        QTreeWidgetItem *v = new QTreeWidgetItem(tree, QStringList() << categoryForTreeWiget() );
        v->setFont(0,bold);
        foreach(const QString &lang, m_Hash_T_Lang.keys()) {
            l = new QTreeWidgetItem(v, QStringList() << "Language" << lang );
            l->setFont(0,bold);
            m_Hash_T_Lang.value(lang).toTreeWidgetItem(l);
        }
    }

private:
    QHash<QString, T> m_Hash_T_Lang;
};

} // end Trans

#endif // MULTILINGUALCLASSTEMPLATE_H
