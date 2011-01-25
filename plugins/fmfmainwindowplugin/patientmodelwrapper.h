/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef PATIENTMODELWRAPPER_H
#define PATIENTMODELWRAPPER_H

#include <coreplugin/ipatient.h>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE


/**
 * \file patientmodelwrapper.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 20 June 2010
*/

namespace Patients {
class PatientModel;
}

namespace MainWin {
namespace Internal {

/** \brief PatientModel wrapper can be accessed using Core::ICore::instance()->patient() */
class PatientModelWrapper : public Core::IPatient
{
    Q_OBJECT
public:
    PatientModelWrapper(Patients::PatientModel *model);
    ~PatientModelWrapper();

    // IPatient interface
    void clear() {}
    bool has(const int ref) const {return (ref>=0 && ref<Core::IPatient::NumberOfColumns);}

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);


    /** \todo Is this needed in freemedforms ? */
    QString toXml() const {return QString();}
    bool fromXml(const QString &) {return true;}

private Q_SLOTS:
    void patientDataChanged(const QModelIndex &index);

private:
    Patients::PatientModel *m_Model;
};

}  // End namespace Internal
}  // End namespace MainWin

#endif // PATIENTMODELWRAPPER_H
