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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef GENERICDATAPACKCREATOR_H
#define GENERICDATAPACKCREATOR_H

#include <coreplugin/ifullreleasestep.h>

/**
 * \file datapackwidget.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 20 Nov 2012
*/

namespace DataPackPlugin {
namespace Internal {

class GenericDataPackCreator : public Core::IFullReleaseStep
{
    Q_OBJECT
public:
    explicit GenericDataPackCreator(QObject *parent = 0);

    virtual QString id() const {return objectName();}
    virtual Steps stepNumber() const;

    virtual bool createTemporaryStorage();
    virtual bool cleanTemporaryStorage();

    bool startProcessing(ProcessTiming timing, SubProcess subProcess);

private:
    bool registerDataPack();

private Q_SLOTS:
    void onSubProcessFinished();

private:
    ProcessTiming _currentTiming;
    SubProcess _currentSubProcess;
};

} // namespace Internal
} // namespace DataPackPlugin

#endif // GENERICDATAPACKCREATOR_H
