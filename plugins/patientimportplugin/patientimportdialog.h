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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Jerome Pinguet <jerome@jerome.cc                       *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTIMPORT_PATIENTIMPORTDIALOG_H
#define PATIENTIMPORT_PATIENTIMPORTDIALOG_H

#include <patientimportplugin/patientimport_exporter.h>
#include <patientimportconstants.h>
#include <qglobal.h>
#include <QDialog>

namespace PatientImport {
namespace Internal {
class PatientImportDialogPrivate;
} // namespace Internal

class PATIENTIMPORT_EXPORT PatientImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PatientImportDialog(QWidget *parent = 0);
    ~PatientImportDialog();

    void setSoftwareComboBoxItems(const QStringList &softlist);

private Q_SLOTS:
    void startImport();

private:
    void changeEvent(QEvent *e);

private:
    Internal::PatientImportDialogPrivate *d;
};

} // namespace PatientImport

#endif // PATIENTIMPORT_PATIENTIMPORTDIALOG_H
