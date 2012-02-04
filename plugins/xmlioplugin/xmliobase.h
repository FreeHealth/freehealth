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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef XMLIOBASE_H
#define XMLIOBASE_H

#include <utils/database.h>
#include <xmlioplugin/xmlformname.h>

#include <QObject>
#include <QDateTime>

/**
 * \file xmliobase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 11 Jan 2012
*/

namespace Form {
class FormIODescription;
class FormIOQuery;
}


namespace XmlForms {
namespace Internal {

class XmlIOBase :  public QObject, public Utils::Database
{
    Q_OBJECT

protected:
    XmlIOBase(QObject *parent = 0);

public:
    // DO NOT MODIFY ORDER (USED IN DATABASE)
    enum TypeOfContent {
        FullContent = 0,
        Description,
        ScreenShot,
        ScriptFile,
        UiFile,
        HtmlFile,
//        Forms,
//        PmhCategories,
        ExtraFiles
    };

    // Constructor
    static XmlIOBase *instance();
    virtual ~XmlIOBase() {}

    // initialize
    bool initialize();
    bool isInitialized() const;

    // Getters
    bool isFormExists(const QString &formUid, const int type = FullContent, const QString &modeName = QString::null);

    QList<Form::FormIODescription *> getFormDescription(const Form::FormIOQuery &query);
    QHash<QString, QString> getAllFormFullContent(const QString &formUid);
    QString getFormContent(const QString &formUid, const int type, const QString &modeName = QString::null);

    QPixmap getScreenShot(const QString &formUid, const QString &shotName);
    QHash<QString, QPixmap> getScreenShots(const QString &formUid, const QString &lang);

    // Setters
    bool saveForm(const XmlFormName &form);
    void saveScreenShots(const XmlFormName &form);
    void saveFiles(const XmlFormName &form, const QString &subDir, const QString &fileExtension, XmlIOBase::TypeOfContent type);

private:
    bool createDatabase(const QString &connectionName, const QString &dbName,
                        const QString &pathOrHostName,
                        TypeOfAccess access, AvailableDrivers driver,
                        const QString &login, const QString &pass,
                        const int port,
                        CreationOption createOption
                       );
    bool checkDatabaseVersion();

    bool saveContent(const QString &formUid, const QString &xmlContent, const int type, const QString &modeName = QString::null, const QDateTime &date = QDateTime::currentDateTime());

private Q_SLOTS:
    void onCoreDatabaseServerChanged();

private:
    static bool m_initialized;
    static XmlIOBase *m_Instance;
};

}  // End namespace Internal
}  // End namespace XmlForms

#endif // XMLIOBASE_H
