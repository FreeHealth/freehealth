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
#ifndef IFIRSTCONFIGURATIONPAGE_H
#define IFIRSTCONFIGURATIONPAGE_H

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QString>
#include <QWizardPage>

/**
 * \file ifirstconfigurationpage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 11 May 2011
 * \class Core::IFirstConfigurationPage
 * \brief Derive object from this interface and set it inside the PluginManager object pool to get your page in the application configurator.
 * You should create the pages in the constructor of your plugin. They should be used before the ool initialize(const QStringList &arguments, QString *errorMessage = 0);
*/

namespace Core {

class CORE_EXPORT IFirstConfigurationPage : public QObject
{
    Q_OBJECT
public:
    enum Pages {
        FirstPage = 0,
        ProxyConfig,
        ServerClientConfig,
        ServerConfig,
        UserDbConnection,
        DatabaseCreationPage,
        UserCreation,
        PatientForm,
        VirtualPatients,
        OtherPage,
        LastPage = 100000
    };
    IFirstConfigurationPage(QObject *parent = 0) : QObject(parent) {}
    virtual ~IFirstConfigurationPage() {}

    virtual int id() const = 0;

    // widget will be deleted after the show
    virtual QWizardPage *createPage(QWidget *parent) = 0;
};

} // namespace Core

#endif // IFIRSTCONFIGURATIONPAGE_H
