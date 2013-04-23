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
#ifndef FORM_PATIENTFORMITEMDATAWRAPPER_H
#define FORM_PATIENTFORMITEMDATAWRAPPER_H

#include <formmanagerplugin/formmanager_exporter.h>
#include <QObject>
#include <QModelIndex>

/**
 * \file patientformitemdatawrapper.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 03 Apr 2013
*/

namespace Form {
namespace Internal {
class PatientFormItemDataWrapperPrivate;
} // namespace Internal

class FORM_EXPORT PatientFormItemDataWrapper : public QObject
{
    Q_OBJECT
    
public:
    explicit PatientFormItemDataWrapper(QObject *parent = 0);
    ~PatientFormItemDataWrapper();
    bool initialize();

    bool isDataAvailable(int ref) const;
    QVariant data(int ref, int role = -1) const;

private Q_SLOTS:
    void onCurrentPatientChanged();
//    void onCurrentPatientFormsLoaded();
    void editingModelEpisodeChanged(const QModelIndex &index);
    void editingModelRowsInserted(const QModelIndex &parent, int first, int last);
    void editingModelRowsRemoved(const QModelIndex &parent, int first, int last);

private:
    Internal::PatientFormItemDataWrapperPrivate *d;
};

} // namespace Form

#endif  // FORM_PATIENTFORMITEMDATAWRAPPER_H

