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
 ***************************************************************************/
#ifndef BASEFORMWIDGESTPLUGIN_H
#define BASEFORMWIDGETSPLUGIN_H

//#include "basewigets_exporter.h"

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>
#include <QPointer>

/**
 * \file baseformplugin.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.4
 * \date 11 Aug 2009
*/

namespace BaseWidgets {
namespace Internal {
class BaseFormWidgetsOptionsPage;
}
class BaseWidgetsFactory;

class BaseWidgetsPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    BaseWidgetsPlugin();
    ~BaseWidgetsPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
private:
    QPointer<BaseWidgetsFactory> m_Factory;
    QPointer<Internal::BaseFormWidgetsOptionsPage> m_OptionsPage;
};


} // End BASEFORMWIDGETSPLUGIN_H

#endif
