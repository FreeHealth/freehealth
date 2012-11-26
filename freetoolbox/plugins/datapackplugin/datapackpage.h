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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACKPLUGIN_INTERNAL_DATAPACKPAGE_H
#define DATAPACKPLUGIN_INTERNAL_DATAPACKPAGE_H

#include <coreplugin/itoolpage.h>
#include <coreplugin/ifullreleasestep.h>

/**
 * \file datapackpage.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 11 Nov 2012
*/

namespace DataPackPlugin {
namespace Internal {
class DataPackPagePrivate;
class DataPackStep;

class DataPackPage : public Core::IToolPage
{
    Q_OBJECT
public:
    explicit DataPackPage(QObject *parent = 0);
    ~DataPackPage();
    bool initialize();
    
    virtual QString id() const {return "DataPackPage";}
    virtual QString name() const;
    virtual QString category() const;
    virtual QIcon icon() const;

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);

private:
    Internal::DataPackPagePrivate *d;
};

class DataPackStep : public Core::IFullReleaseStep
{
    Q_OBJECT

public:
    DataPackStep(QObject *parent = 0);
    ~DataPackStep();

    QString id() const {return "DataPackStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::DataPackProcessing;}

    bool createTemporaryStorage();
    bool cleanTemporaryStorage();

    bool startDownload(QProgressBar *bar = 0);
    bool process();
    QString processMessage() const;

    bool registerDataPack();

    QStringList errors() const {return m_Errors;}

private:
    QStringList m_Errors;
    bool m_WithProgress;
};

} // namespace Internal
} // namespace DataPackPlugin

#endif // DATAPACKPLUGIN_INTERNAL_DATAPACKPAGE_H

