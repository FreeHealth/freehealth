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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTBASECOMPLETER_H
#define PATIENTBASECOMPLETER_H

#include <patientbaseplugin/patientbase_exporter.h>
#include <utils/widgets/qbuttonlineedit.h>

#include <QCompleter>
#include <QValidator>

class QSqlTableModel;

/**
 * \file patientlineeditcompletersearch.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 05 Jul 2011
*/

namespace Patients {
namespace Internal {
class PatientBaseCompleterPrivate;

class PatientBaseCompleter : public QCompleter
{
    Q_OBJECT
public:
    enum CompleterModelRepresentation {
        FullName = 0,
        Uid
    };

    explicit PatientBaseCompleter(QObject *parent = 0);
    ~PatientBaseCompleter();

    QValidator *validator() const;

private:
    Internal::PatientBaseCompleterPrivate *d;
};
}  // End namespace Internal

class PATIENT_EXPORT PatientLineEditCompleterSearch : public Utils::QButtonLineEdit
{
    Q_OBJECT
public:
    explicit PatientLineEditCompleterSearch(QWidget *parent = 0);
    ~PatientLineEditCompleterSearch();

Q_SIGNALS:
    void selectedPatient(const QString &uid, const QString &fullName);

private Q_SLOTS:
    void textChanged(const QString &newText);
    void cancelSearch();
    void patientSelected(const QModelIndex &index);

//private:
//    void keyPressEvent(QKeyEvent *event);

private:
    QString m_LastSearch;
    Internal::PatientBaseCompleter *m_Completer;
};

}

#endif // PATIENTBASECOMPLETER_H
