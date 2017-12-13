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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "xmlformname.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <QDir>
#include <QFileInfo>

#include <QDebug>

using namespace XmlForms;
using namespace Internal;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

XmlFormName::XmlFormName() :
    isValid(false),
    isAvailableFromDatabase(false),
    dbId(-1)
{
}

XmlFormName::XmlFormName(const QString &_uid) :
    isValid(false),
    isAvailableFromDatabase(false),
    uid(_uid),
    absFileName(_uid),
    dbId(-1)
{
    if (uid.endsWith(".xml", Qt::CaseInsensitive)) {
        int begin = uid.lastIndexOf("/");
        uid = uid.left(begin);
        modeName = _uid.mid(begin+1);
        if (modeName.endsWith(".xml"))
            modeName.chop(4);
    }

    // Replace tags with full path
    if (uid.startsWith(QString(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH).left(2))) {
        absFileName.replace(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::CompleteFormsPath));
        absFileName.replace(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH, settings()->path(Core::ISettings::SubFormsPath));
        absFileName.replace(Core::Constants::TAG_APPLICATION_RESOURCES_PATH, settings()->path(Core::ISettings::BundleResourcesPath));
        absFileName.replace(Core::Constants::TAG_APPLICATION_USER_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::UserCompleteFormsPath));
        absFileName.replace(Core::Constants::TAG_APPLICATION_USER_SUBFORMS_PATH, settings()->path(Core::ISettings::SubFormsPath));
        absFileName.replace(Core::Constants::TAG_APPLICATION_LOCAL_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::LocalCompleteFormsPath));
        absFileName.replace(Core::Constants::TAG_APPLICATION_LOCAL_SUBFORMS_PATH, settings()->path(Core::ISettings::LocalSubFormsPath));
        absFileName.replace(Core::Constants::TAG_DATAPACK_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::DataPackCompleteFormsInstallPath));
        absFileName.replace(Core::Constants::TAG_DATAPACK_SUBFORMS_PATH, settings()->path(Core::ISettings::DataPackSubFormsInstallPath));
    } else {
        // uuid was a full abs path --> recreate the tags
        uid.replace(settings()->path(Core::ISettings::DataPackCompleteFormsInstallPath), Core::Constants::TAG_DATAPACK_COMPLETEFORMS_PATH);
        uid.replace(settings()->path(Core::ISettings::DataPackSubFormsInstallPath), Core::Constants::TAG_DATAPACK_SUBFORMS_PATH);
        uid.replace(settings()->path(Core::ISettings::CompleteFormsPath), Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH);
        uid.replace(settings()->path(Core::ISettings::SubFormsPath), Core::Constants::TAG_APPLICATION_SUBFORMS_PATH);
        uid.replace(settings()->path(Core::ISettings::UserCompleteFormsPath), Core::Constants::TAG_APPLICATION_USER_COMPLETEFORMS_PATH);
        uid.replace(settings()->path(Core::ISettings::UserSubFormsPath), Core::Constants::TAG_APPLICATION_USER_SUBFORMS_PATH);
        uid.replace(settings()->path(Core::ISettings::LocalCompleteFormsPath), Core::Constants::TAG_APPLICATION_LOCAL_COMPLETEFORMS_PATH);
        uid.replace(settings()->path(Core::ISettings::LocalSubFormsPath), Core::Constants::TAG_APPLICATION_LOCAL_SUBFORMS_PATH);
        uid.replace(settings()->path(Core::ISettings::BundleResourcesPath), Core::Constants::TAG_APPLICATION_RESOURCES_PATH);
    }

    absFileName = QDir::cleanPath(absFileName);
    QFileInfo info(absFileName);
    if (info.isDir() || info.suffix().compare("xml", Qt::CaseInsensitive)!=0) {
        absPath = absFileName;
        if (modeName.isEmpty())
            absFileName += "/central.xml";
        else
            absFileName += "/" + modeName + ".xml";
        info.setFile(absFileName);
    } else {
        absPath = info.absolutePath();
    }
    isValid = info.exists();

//    qWarning() << "\n\n" << absFileName << absPath << isValid << modeName << "\n\n";
}

bool XmlFormName::operator==(const XmlFormName &other) const
{
    return (uid==other.uid
            && isValid==other.isValid
            && absFileName==other.absFileName
            && absPath==other.absPath
            && modeName==other.modeName);
}

QDebug XmlForms::Internal::operator<<(QDebug dbg, const XmlFormName &c)
{
    dbg.nospace() << "XmlFormName(Valid:" << c.isValid
                  << "; Uid: " << c.uid
                  << "; Mode: " << c.modeName
                  << "; File: " << c.absFileName
                  << "; AbsPath: " << c.absPath
                  << ")";
    return dbg.space();
}
