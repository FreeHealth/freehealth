/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
/**
   \class MainWin::Internal::PatientModelWrapper
   Wrapper to the Patient::PatientModel for identity and wrapper to Form::FormItem that represent
   a patient data.
*/

#include "patientmodelwrapper.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemdata.h>

#include <patientbaseplugin/patientmodel.h>

static inline Form::FormManager *formManager() {return Form::FormManager::instance();}
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }

using namespace MainWin::Internal;

PatientModelWrapper::PatientModelWrapper(Patients::PatientModel *model) :
        Core::IPatient(model), m_Model(model)
{
    connect(model, SIGNAL(patientChanged(QString)), this, SLOT(onCurrentPatientChanged(QString)));
}

PatientModelWrapper::~PatientModelWrapper()
{
    // remove this wrapper from the core instance
    Core::ICore::instance()->setPatient(0);
}

void PatientModelWrapper::onCurrentPatientChanged(const QString &)
{
    qWarning() << Q_FUNC_INFO;
    Q_EMIT currentPatientChanged();
}

/** \brief Initialize the model */
void PatientModelWrapper::init()
{
}

/** \brief Return the QModelIndex of the current patient. The index is parented with the Patient::PatientModel model. */
QModelIndex PatientModelWrapper::currentPatientIndex() const
{
    return m_Model->currentPatient();
}

/**
  \brief Return the Patient's Data represented by the \e index.column().
  The wrapper model searches in the identity model (Patient::PatientModel) and if it does not found
  value, searches in the Forms (some Form::FormItem can represent patient values).
*/
QVariant PatientModelWrapper::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    // get datas from the model
    QModelIndex idx = m_Model->index(index.row(), index.column());
    QVariant result = idx.data();
    if (!result.isNull())
        return result;

    // or in the forms widgets
    foreach(Form::FormMain *modeForms, formManager()->forms()) {
        foreach(Form::FormMain *f, modeForms->flattenFormMainChildren()) {
            foreach(Form::FormItem *item, f->formItemChildren()) {
                if (item->itemDatas()) {
                    if (item->patientDataRepresentation() == index.column())
                        return item->itemDatas()->data(item->patientDataRepresentation(), Form::IFormItemData::ID_ForPatientModel);
                }
            }
        }
    }
    return QVariant();
}

/**
  \brief Return the Patient's Data represented by the \e column.
  The wrapper model searches in the identity model (Patient::PatientModel) and if it does not found
  value, searches in the Forms (some Form::FormItem can represent patient values).
*/
QVariant PatientModelWrapper::data(int column) const
{
    QModelIndex idx = m_Model->index(m_Model->currentPatient().row(), column);
    return this->data(idx);
}

bool PatientModelWrapper::setValue(int ref, const QVariant &value)
{
    QModelIndex idx = m_Model->index(m_Model->currentPatient().row(), ref);
    return setData(idx, value);
}

bool PatientModelWrapper::setData(const QModelIndex &item, const QVariant &value, int role)
{
    QModelIndex idx = m_Model->index(item.row(), item.column());
    if (m_Model->setData(idx, value, role)) {
        Q_EMIT dataChanged(idx, idx);
        return true;
    }
    return false;
}

void PatientModelWrapper::hidePatientBar()
{
    Patients::PatientBar::instance()->hide();
}

void PatientModelWrapper::showPatientBar()
{
    Patients::PatientBar::instance()->show();
}

/** \brief Private connection to the Patient::PatientModel source. */
void PatientModelWrapper::patientDataChanged(const QModelIndex &index)
{
    if (m_Model->currentPatient().row() == index.row())
        Q_EMIT dataChanged(index, index);
}
