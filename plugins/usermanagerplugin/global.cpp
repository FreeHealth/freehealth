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
#include <usermanagerplugin/global.h>
#include <usermanagerplugin/database/userbase.h>
#include <usermanagerplugin/usermodel.h>

#include <coreplugin/translators.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>

#include <QCryptographicHash>
#include <QByteArray>

namespace UserPlugin {

void initLib()
{
//    tkLog::addMessage( "tkUserGlobal", "Initializing tkUser lib" );
//    // load translator
//    if ( tkTranslators::instance() )
//        tkTranslators::instance()->addNewTranslator( "usertoolkit" );
//    // initialize database
//    Q_ASSERT_X( tkSettings::instance() , "tkSettings", "Before calling tkUserGlobal::init() YOU MUST define an instance of tkSettings" );
////    tkUserBase::instance()->initialize( tkSettings::instance() );
//    // initialize model
//    tkUserModel::instance();
//    tkLog::addMessage( "tkUserGlobal", "tkUser lib is initialized" );
}

QString getLibraryInformations()
{
    return QString( "user toolkit library, version : %1 %2 , compiled on : %3 %4" )
            .arg( PACKAGE_VERSION )
#ifdef DEBUG
            .arg( "debug" )
#else
            .arg( "release" )
#endif
            .arg( __DATE__ )
            .arg( __TIME__ );
}


}
