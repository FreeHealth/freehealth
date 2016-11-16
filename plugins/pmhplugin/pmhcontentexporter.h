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
/***************************************************************************
 *  Authors:                                                               *
 *       Eric MAEKER <eric.maeker@gmail.com>                               *
 *                                                                         *
 ***************************************************************************/
#pragma once

#include <coreplugin/ipatientdataexporter.h>
#include <QObject>

/**
 * \file pmhcontentexporter.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 22 Apr 2013
*/

namespace PMH {
namespace Internal {
class PmhContentExporterPrivate;

class PmhContentExporter : public Core::IPatientDataExporter
{
    Q_OBJECT
    
public:
    explicit PmhContentExporter(QObject *parent = 0);
    ~PmhContentExporter();
    
    bool initialize();
    bool isBusy() const;

public Q_SLOTS:
    Core::PatientDataExtraction *startExportationJob(const Core::PatientDataExporterJob &job);

private:
    PmhContentExporterPrivate *d;
};

} // namespace Internal
} // namespace PMH

