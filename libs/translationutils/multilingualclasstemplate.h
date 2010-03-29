/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef MULTILINGUALCLASSTEMPLATE_H
#define MULTILINGUALCLASSTEMPLATE_H

#include <translationutils/constanttranslations.h>

#include <QString>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QHash>

namespace Trans {

/**
  \brief This template can be used to manage a multilingual data class.
  It manages a default set for all language, and one for each languages.

  \code
// Declare a class
class MyMultiLingualClass : public Trans::MultiLingualClass<OneDataPerLangClass>
{
public:
    MyMultiLingualClass() {}
    ~MyMultiLingualClass() {}

    QString categoryForTreeWiget() const {return QString("My Text to show for the debug dialog");}
};

// Then is the MyMultiLingualClass find your datas by lang name
// lang should be 2 char locale
if (d->hasLanguage(lang))
    s = d->getLanguage(lang);
else
    s = d->createLanguage(lang);
  \endcode
*/
template <typename T>
class MultiLingualClass
{
public:
    MultiLingualClass() { }
    virtual ~MultiLingualClass()
    {
        qDeleteAll(m_Hash_T_Lang);
        m_Hash_T_Lang.clear();
    }

    /** Use for the debugging treewidget construction */
    virtual QString categoryForTreeWiget() const = 0;

    /** Check if the \e lang already exists */
    bool hasLanguage(const QString &lang)
    {
        return m_Hash_T_Lang.contains(lang.left(2));
    }

    /**
        Return a pointer to the datas class for one unique \e language, or if it does not exist for
        the default common language.
        If \e language doesnot exists return 0.
     */
    T *getLanguage(const QString &language)
    {
        QString tmp = language.left(2);
        T *s = 0;
        if (m_Hash_T_Lang.contains(tmp)) {
            s = m_Hash_T_Lang.value(tmp);
        } else {
            if (m_Hash_T_Lang.contains(Constants::ALL_LANGUAGE))
                s = m_Hash_T_Lang.value(Constants::ALL_LANGUAGE);
        }
        return s;
    }

    /** Return a pointer to the datas class for one unique \e language. If \e language doesnot exists it is created. */
    T *createLanguage(const QString &lang)
    {
        QString tmp = lang.left(2);
        T *s = 0;
        if (m_Hash_T_Lang.contains(tmp)) {
            s = m_Hash_T_Lang.value(tmp);
        } else {
            s = new T;
            m_Hash_T_Lang.insert(tmp,s);
        }
        return s;
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
            T *s = m_Hash_T_Lang.value(lang);
            s->toTreeWidgetItem(l);
        }
    }

private:
    QHash<QString, T *> m_Hash_T_Lang;
};

} // end Trans

#endif // MULTILINGUALCLASSTEMPLATE_H
