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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTMODELWRAPPER_H
#define PATIENTMODELWRAPPER_H

#include <coreplugin/ipatient.h>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file patientmodelwrapper.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 12 Sept 2012
*/

namespace Patients {
class PatientModel;
namespace Internal {

/** \brief PatientModel wrapper can be accessed using Core::ICore::instance()->patient() */
class PatientModelWrapper : public Core::IPatient
{
    Q_OBJECT
public:
    PatientModelWrapper(QObject *parent = 0);
    ~PatientModelWrapper();
    void initialize(Patients::PatientModel *model);
    Patients::PatientModel *patientModel() {return m_Model;}

    // IPatient interface
    void clear() {}
    bool has(const int ref) const {return (ref>=0 && ref<Core::IPatient::NumberOfColumns);}
    QModelIndex currentPatientIndex() const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant data(int column) const;

    /** \deprecated **/
    virtual bool setValue(int ref, const QVariant &value); // TODO: remove this and use setData instead
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    //TODO: Is this needed in freemedforms?
    QString toXml() const {return QString();}
    bool fromXml(const QString &) {return true;}

    Core::IPatientBar *patientBar() const;
    virtual void hidePatientBar();
    virtual void showPatientBar();
    virtual bool isPatientBarVisible() const;

    virtual QHash<QString, QString> fullPatientName(const QString &uuid) const;
    virtual QHash<QString, QString> fullPatientName(const QStringList &uuids) const;

private Q_SLOTS:
    bool submit();
    void onCurrentPatientChanged(const QString &);
    void patientDataChanged(const QModelIndex &index);

private:
    Patients::PatientModel *m_Model;
};

}  // End namespace Internal
}  // End namespace MainWin

#endif // PATIENTMODELWRAPPER_H
