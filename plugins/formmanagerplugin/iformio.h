/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#ifndef IFORMIO_H
#define IFORMIO_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <translationutils/multilingualclasstemplate.h>
#include <translationutils/constanttranslations.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <QHash>
#include <QVariant>

/**
 * \file iformio.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 15 Mar 2011
*/

namespace Form {
class FormMain;

class FORM_EXPORT FormIODescription
{
public:
    enum DataRepresentation {
        Author = 0,
        Country,
        AvailableLanguages,
        Version,
        CreationDate,
        LastModificationDate,
        ShortDescription,
        HtmlDescription,
        License,
        GeneralIcon
    };

    FormIODescription();
    virtual ~FormIODescription();

    QVariant data(const int ref, const QString &lang = QString::null) const;
    bool setData(const int ref, const QVariant &value, const QString &lang = QString::null);

    void formDescriptionToTreeWidget(QTreeWidget *tree, const QString &lang = QString::null) const;

private:
    mutable Trans::MultiLingualClass< QHash<int, QVariant> > m_Datas;
};


class FORM_EXPORT IFormIO : public QObject
{
    Q_OBJECT
public:
    IFormIO(const QString &absFileName, QObject *parent=0) : QObject(parent) { Q_UNUSED(absFileName); }
    virtual ~IFormIO() {}

    virtual QString name() const = 0;

    virtual void muteUserWarnings(bool state) = 0;

    virtual bool setFileName(const QString &absFileName) = 0;

    virtual QStringList fileFilters() const = 0;

    virtual QString managedFileExtension() const = 0;
    virtual bool canReadFile() const = 0;

    // canReadFile() must be called first, no need to loadForm to get these informations
//    virtual FormIODescription readFileInformations() = 0;
    /** \todo code here, this part should be obsolete */
    virtual bool readFileInformations() = 0;
    virtual QString formAuthor() const = 0;
    virtual QString formVersion() const = 0;
    virtual QString formDescription(const QString &lang = Trans::Constants::ALL_LANGUAGE) const = 0;
    virtual void formDescriptionToTreeWidget(QTreeWidget *tree, const QString &lang = Trans::Constants::ALL_LANGUAGE) const = 0;
    /** end */

    virtual Form::FormMain *loadForm() = 0;
    virtual bool saveForm(QObject *treeRoot) = 0;

    virtual QString lastError() const = 0;
};

} // end Form

#endif // IFORMIO_H
