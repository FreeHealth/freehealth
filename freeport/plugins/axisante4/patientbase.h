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
 *   Main Developer: Fredman, Eric Maeker <eric.maeker@gmail.com>          *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AXISANTE4_PATIENTBASE_H
#define AXISANTE4_PATIENTBASE_H

#include <QObject>
#include <QString>

namespace AxiSante4 {

class PatientBase : public QObject
{
    Q_OBJECT
public:
    PatientBase(QObject *parent);
    bool initialize();

    void setDatabasePath(const QString &absFilePath);

    bool startDataExtraction();
    bool extractNextPatient();

private:
    QString _dbFilePath;
};

} // namespace AxiSante4

#endif // AXISANTE4_PATIENTBASE_H
