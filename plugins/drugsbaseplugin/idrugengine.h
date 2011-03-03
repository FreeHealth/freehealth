/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#ifndef DRUGSDB_IDRUGENGINE_H
#define DRUGSDB_IDRUGENGINE_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <QObject>
#include <QIcon>

/**
 * \file idrugengine.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 04 Mar 2011
*/

namespace DrugsDB {
class IDrugInteraction;
class IDrug;

class IDrugEngine : public QObject
{
    Q_OBJECT

public:
    IDrugEngine(QObject *parent) : QObject(parent) {}
    virtual ~IDrugEngine() {}

    virtual bool init() = 0;

    bool isActive() const {return m_IsActive;}
    void setActive(bool state) {m_IsActive = state;}
    virtual bool canComputeInteractions() const = 0;

    virtual bool isCalculatingDrugDrugInteractions() const = 0;
    virtual bool isCalculatingPatientDrugInteractions() const = 0;

    virtual QString name() const = 0;
    virtual QString uid() const = 0;

    virtual int calculateInteractions(const QVector<IDrug *> &drugs) = 0;
    virtual QVector<IDrugInteraction *> getAllInteractionsFound() = 0;

    /** For size refer to Core::ITheme::IconSize. For \e levelOfWarning refer to DrugsDb::Constants::LevelOfInteractionWarnings */
    virtual QIcon maximumInteractingLevelIcon(const QVector<IDrugInteraction *> &interactions, const IDrug *drug, const int levelOfWarning, const int size) = 0;

private:
    bool m_IsActive;
};

}  // End namespace DrugsDB


#endif // DRUGSDB_IDRUGENGINE_H
