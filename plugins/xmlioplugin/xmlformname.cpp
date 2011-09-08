/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "xmlformname.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <QDir>
#include <QFileInfo>

using namespace XmlForms;
using namespace Internal;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

XmlFormName::XmlFormName(const QString &_uid) :
    isValid(false), uid(_uid), absFileName(_uid)
{
    if (uid.endsWith(".xml", Qt::CaseInsensitive)) {
        uid = uid.left(uid.lastIndexOf("/"));
    }
    if (uid.startsWith(QString(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH).left(2))) {
        absFileName.replace(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::CompleteFormsPath));
        absFileName.replace(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH, settings()->path(Core::ISettings::SubFormsPath));
        absFileName.replace(Core::Constants::TAG_APPLICATION_RESOURCES_PATH, settings()->path(Core::ISettings::BundleResourcesPath));
    }
    uid.replace(settings()->path(Core::ISettings::CompleteFormsPath), Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH);
    uid.replace(settings()->path(Core::ISettings::SubFormsPath), Core::Constants::TAG_APPLICATION_SUBFORMS_PATH);
    uid.replace(settings()->path(Core::ISettings::BundleResourcesPath), Core::Constants::TAG_APPLICATION_RESOURCES_PATH);
    absFileName = QDir::cleanPath(absFileName);
    isValid = QFileInfo(absFileName).exists();
}
