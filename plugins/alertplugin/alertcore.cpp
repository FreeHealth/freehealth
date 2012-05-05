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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "alertcore.h"
#include "alertbase.h"
#include "alertmanager.h"

#include <QUuid>
#include <QString>

using namespace Alert;

AlertCore *AlertCore::m_singleton = 0;

AlertCore *AlertCore::instance(QObject *parent)
{
    if (!m_singleton)
        m_singleton = new AlertCore(parent);
    return m_singleton;
}

AlertCore::AlertCore(QObject *parent) :
    QObject(parent),
    m_alertBase(0),
    m_alertManager(0)
{
}

AlertCore::~AlertCore()
{
    // QObject manages children deletion so no need to delete the singleton
}

void AlertCore::initialize()
{
    // Create all instance
    m_alertBase = new AlertBase(this);
    m_alertManager = new AlertManager(this);
}

QString AlertCore::setAlertUuid()
{
  return QUuid::createUuid().toString();
}

void AlertCore::showIHMaccordingToType(int type)
{
    m_alertManager->initializeWithType(type);
}
