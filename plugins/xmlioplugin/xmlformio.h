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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef XMLFORMIO_H
#define XMLFORMIO_H

#include <formmanagerplugin/iformio.h>
#include <xmlioplugin/xmlformname.h>

#include <QDomNode>
#include <QObject>
#include <QCache>

/**
 * \file xmlformio.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 05 Sept 2011
*/

namespace Category {
class CategoryItem;
}

namespace Form {
class FormItem;
class FormMain;
class IFormWidgetFactory;
}

namespace XmlForms {
namespace Internal {

class XmlFormIO : public Form::IFormIO
{
     Q_OBJECT
public:
    XmlFormIO(QObject *parent=0);
    ~XmlFormIO();

    static QString lastestXmlVersion();

    // Form::IForm interface
    QString name() const {return "XmlFormIO";}

    void muteUserWarnings(bool state) {m_Mute = state;}

    bool canReadForms(const QString &uuidOrAbsPath) const;
    bool canReadForms(const Form::FormIOQuery &query) const;
//    bool canReadScripts(const Form::FormIOQuery &query) const;

    Form::FormIODescription *readFileInformation(const QString &uuidOrAbsPath) const;
    QList<Form::FormIODescription *> getFormFileDescriptions(const Form::FormIOQuery &query) const;

    QList<Form::FormMain *> loadAllRootForms(const QString &uuidOrAbsPath = QString::null) const;
    bool loadPmhCategories(const QString &uuidOrAbsPath) const;

    QList<QPixmap> screenShots(const QString &uuidOrAbsPath) const;
    QPixmap screenShot(const QString &uuidOrAbsPath, const QString &name) const;

    bool saveForm(QObject *treeRoot) { Q_UNUSED(treeRoot); return true; }

    QString lastError() const {return m_Error.join("\n");}

    void checkForUpdates() const;
    // End Form::IForm interface

private:
//    void getAllFormsFromDir(const QString &absPath, QList<Form::FormIODescription *> *list) const;
    bool createCategory(const QDomElement &element, Category::CategoryItem *parent, const QString &readingAbsPathFile) const;

public:
    bool checkDatabaseFormFileForUpdates() const;

private:
     mutable QString m_AbsFileName;
     mutable QStringList m_Error;
     mutable QDomDocument m_MainDoc;
     bool m_Mute;
     mutable Form::FormMain *m_ActualForm;

     // Caching some data for speed improvements
     mutable QHash<QString, bool> m_ReadableForms;
     mutable QHash<QString, bool> m_ReadableScripts;

     // Form UUID cache
     mutable QHash<QString, XmlFormName> m_FormNames;
};

}  // End namespace Internal
}  // End namespace XmlForms

#endif  //  XMLFORMIO_H
