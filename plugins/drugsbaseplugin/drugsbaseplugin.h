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
 ***************************************************************************/
#ifndef DRUGSBASE_PLUGIN_H
#define DRUGSBASE_PLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>

/**
 * \file drugsbaseplugin.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.1
 * \date 25 Oct 2009
*/

namespace DrugsDB {
    namespace Internal {
        class DrugDrugInteractionEngine;
        class PimEngine;
    }

class DrugsBasePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    DrugsBasePlugin();
    ~DrugsBasePlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

private:
    DrugsDB::Internal::DrugDrugInteractionEngine *m_DDIEngine;
    DrugsDB::Internal::PimEngine *m_PimEngine;
};


} // End DRUGSBASE_PLUGIN_H

#endif
