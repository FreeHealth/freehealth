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
#ifndef FORM_FORMEXPORTER_H
#define FORM_FORMEXPORTER_H

#include <coreplugin/ipatientdataexporter.h>
#include <QObject>
#include <QString>
#include <QStringList>

/**
 * \file formexporter.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 20 Apr 2013
*/

namespace Form {
namespace Internal {
class FormExporterPrivate;

class FormExporter : public Core::IPatientDataExporter
{
    Q_OBJECT
    
public:
    explicit FormExporter(bool identityOnly, QObject *parent = 0);
    ~FormExporter();
    bool initialize();

    void setIdentityOnly(bool identityOnly);

    bool isBusy() const;
    
public Q_SLOTS:
    Core::PatientDataExtraction *startExportationJob(const Core::PatientDataExporterJob &job);

private:
    Internal::FormExporterPrivate *d;
};

} // namespace Internal
} // namespace Form

#endif  // FORM_FORMEXPORTER_H

