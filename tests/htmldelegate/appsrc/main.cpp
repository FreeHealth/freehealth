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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:                                                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "mainwindow.h"

#include <datapackutils/datapackcore.h>
#include <datapackutils/iservermanager.h>

#include <utils/global.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

// Sur MacOsX : créé un compte FTP Anonyme
// sudo dscl . -create /Users/ftp
// sudo dscl . -create /Users/ftp NFSHomeDirectory /path/to/ftp/folder
// Effacer compte anonyme
// sudo dscl . -delete /Users/ftp

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("datapack");
    a.setApplicationVersion("0.7.0");

    MainWindow w;
    w.show();
    Utils::centerWidget(&w, qApp->desktop());

    return a.exec();
}
