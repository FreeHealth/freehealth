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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
#include "icore.h"

#include <utils/global.h>

#include <QApplication>
#include <QString>

/*!
    \namespace Core
    \brief The Core namespace contains all classes that make up the Core plugin
    which constitute the basic functionality of FreeMedForms.
*/

/*!
    \namespace Core::Internal
    \internal
*/

/*!
    \class Core::ICore
    \brief The ICore class allows access to the different part that make up
    the basic functionality of FreeMedForms.

    You should never create a subclass of this interface. The one and only
    instance is created by the Core plugin. You can access this instance
    from your plugin through \sa Core::instance().

    \mainclass
*/

Core::ICore::ICore(QObject *parent) :
        QObject(parent), m_UpdatePreferences(false)
{
    alphaDialog();
}

Core::ICore::~ICore()
{}

bool Core::ICore::alphaDialog() const
{
    if (Utils::isAlpha() && !Utils::isDebugWithoutInstallCompilation()) {
        Utils::warningMessageBox("<p style=\"text-transform:uppercase;font-weight:bold;color:red;font-size:large\">" +
                                 tr("You are running an alpha version of %1. This version "
                                    "can be heavily buggy and is only provided for "
                                    "testing purpose.")
                                 .arg(qApp->applicationName() + " (" + qApp->applicationVersion() + ")") +
                                 "</p>"
                                 ,
                                 tr("If you found any problem with %1, please report "
                                    "it on our mailing list: "
                                    "<a href=\"mailto:freemedforms@googlegroups.com>freemedforms@googlegroups.com</a>")
                                 .arg(qApp->applicationName()), "",
                                 tr("Warning alpha version")
                                 );
    }
    return true;
}
