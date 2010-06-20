/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "patientmodelwrapper.h"

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemdata.h>

#include <patientbaseplugin/patientmodel.h>

static inline Form::FormManager *formManager() {return Form::FormManager::instance();}

using namespace MainWin::Internal;

PatientModelWrapper::PatientModelWrapper(Patients::PatientModel *model) :
        Core::IPatient(model), m_Model(model)
{
    connect(model, SIGNAL(patientChanged(QString)), this, SIGNAL(currentPatientChanged()));
}

PatientModelWrapper::~PatientModelWrapper()
{
}

QVariant PatientModelWrapper::value(const int ref) const
{
    // get datas from the model
    QVariant result = m_Model->data(m_Model->index(m_Model->currentPatient().row(), ref));
    if (!result.isNull())
        return result;
    foreach(Form::FormMain *f, formManager()->forms()) {
        foreach(Form::FormItem *item, f->formItemChildren()) {
            if (item->itemDatas()) {
                if (item->patientDataRepresentation() == ref)
                    return item->itemDatas()->data(item->patientDataRepresentation(), Form::IFormItemData::ID_ForPatientModel);
            }
        }
    }
    return QVariant();
}

bool PatientModelWrapper::setValue(const int ref, const QVariant &value)
{
    qWarning() << " PatientModelWrapper::setValue" << ref << value;
    if (!has(ref))
        return false;
    if (m_Model->setData(m_Model->index(m_Model->currentPatient().row(), ref), value)) {
        Q_EMIT dataChanged(ref);
        return true;
    }
    return false;
}

void PatientModelWrapper::patientDataChanged(const QModelIndex &index)
{
    if (m_Model->currentPatient().row() == index.row())
        Q_EMIT this->dataChanged(index.column());
}
