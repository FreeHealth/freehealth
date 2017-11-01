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
#ifndef DATAPACKPLUGIN_INTERNAL_SERVERCREATIONPAGE_H
#define DATAPACKPLUGIN_INTERNAL_SERVERCREATIONPAGE_H

#include <coreplugin/itoolpage.h>
#include <coreplugin/ifullreleasestep.h>

/**
 * \file servercreationpage.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 11 Nov 2012
*/

namespace DataPackPlugin {
namespace Internal {
class ServerCreationPagePrivate;
class ServerCreationStep;

class ServerCreationPage : public Core::IToolPage
{
    Q_OBJECT
public:
    explicit ServerCreationPage(QObject *parent = 0);
    ~ServerCreationPage();
    bool initialize();

    virtual QString id() const {return "ServerCreationPage";}
    virtual QString name() const;
    virtual QString category() const;
    virtual QIcon icon() const;

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);

private:
    Internal::ServerCreationPagePrivate *d;
};

class ServerCreationStep : public Core::IFullReleaseStep
{
    Q_OBJECT

public:
    ServerCreationStep(QObject *parent = 0);
    ~ServerCreationStep();

    QString id() const {return "ServerCreationStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::DataPackProcessing;}

    bool createTemporaryStorage();
    bool cleanTemporaryStorage();

    bool startProcessing(ProcessTiming timing, SubProcess subProcess);

private Q_SLOTS:
    void onSubProcessFinished();

private:
    ProcessTiming _currentTiming;
    SubProcess _currentSubProcess;
};

} // namespace Internal
} // namespace DataPackPlugin

#endif // DATAPACKPLUGIN_INTERNAL_SERVERCREATIONPAGE_H

