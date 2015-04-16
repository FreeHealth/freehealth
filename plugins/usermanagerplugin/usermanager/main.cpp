/***************************************************************************
 *   FreeMedForms Project                                                  *
 *   (C) 2008-2015 by Eric MAEKER, MD                            *
 *   eric.maeker@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <QApplication>

#include <tkUserIdentifier.h>
#include <UserManagerMainWindow.h>
#include <tkUserGlobal.h>

#include <tkGlobal.h>
#include <tkSettings.h>
#include <tkTranslators.h>
#include <tkActionManager.h>
#include <tkContextManager.h>

#include <QDebug>
#include <QFileDialog>

/**
  \brief This project is the FreeMedForms' usermanager application
  \author Eric MAEKER, MD <eric.maeker@gmail.com>
*/

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName( QCoreApplication::translate("main", "FreeMedForms    Users Manager") );
    app.setApplicationVersion( "0.0.2" );

    // init settings
    tkSettings settings;

    // ask user for freemedforms' resources path (try to find user.db)
    QString tmp;
    if (!tkGlobal::isDebugCompilation()) {
        tmp = QFileDialog::getOpenFileName(0, QApplication::translate("main","Select FreeMedForms' binary"),
                                           tkSettings::instance()->path(tkSettings::BundleRootPath));
        if (tmp.isEmpty())
            return 0;

        if (tkGlobal::isRunningOnMac())
            tmp += "/Contents/Resources/";
        else
            tmp += "/Resources";
        tkSettings::instance()->setPath( tkSettings::BundleResourcesPath, tmp );
        tkSettings::instance()->setPath( tkSettings::ResourcesPath, tmp );
    }

    // add translators (if not there will be a segfault)
    tkTranslators *trans = tkTranslators::instance(qApp);
    trans->setPathToTranslations( settings.path( tkSettings::TranslationsPath ) );
    tkUserGlobal::initLib();
    tkGlobal::initLib();
    trans->addNewTranslator( "qt" );

    // identification
    tkUserIdentifier tki;//( QStringList() << QCoreApplication::translate( "main", "Nothing to tell" ) );
    if ( tki.exec() == QDialog::Rejected )
        return 123;

    // user manager
    UserManagerMainWindow *tkm = new UserManagerMainWindow();
    tkActionManager::instance(tkm);
    tkContextManager::instance(tkm);
    tkm->initialize();
    tkm->show();

    return app.exec();
}
