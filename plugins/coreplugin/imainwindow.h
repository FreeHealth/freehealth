/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IMAINWINDOW_H
#define IMAINWINDOW_H

#include <coreplugin/actionmanager/mainwindowactionhandler.h>
#include <coreplugin/core_exporter.h>


#include <QObject>
#include <QIcon>
#include <QPointer>

/**
 * \file imainwindow.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 18 Oct 2011
*/

namespace Core {

class CORE_EXPORT IMainWindow : public Internal::MainWindowActionHandler
{
    Q_OBJECT
public:
    IMainWindow(QWidget *parent = 0) : MainWindowActionHandler(parent) {}
    virtual ~IMainWindow() {}

    virtual bool initialize(const QStringList &arguments, QString *errorString) = 0;
    virtual void extensionsInitialized() = 0;

//    virtual void postCoreInitialization() = 0;

    void startProcessingSpinner();
    void endProcessingSpinner();
};

} // end Core

#endif // IFORMIO_H
