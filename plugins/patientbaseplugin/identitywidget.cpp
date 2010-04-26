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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "identitywidget.h"
#include "patientmodel.h"

#include "ui_identitywidget.h"

#include <QDataWidgetMapper>
#include <QDebug>

using namespace Patients;

namespace Patients {
namespace Internal {
class IdentityWidgetPrivate : public QObject
{
    Q_OBJECT

public:
    IdentityWidgetPrivate(IdentityWidget *parent) :
            ui(new Ui::IdentityWidget), m_Mapper(0), q(parent)
    {
        ui->setupUi(q);
//        connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(on_nextButton_clicked()));
//        connect(ui->prevButton, SIGNAL(clicked()), this, SLOT(on_prevButton_clicked()));
    }

    ~IdentityWidgetPrivate()
    {
        delete ui;
        delete m_Mapper;
    }


    void createMapper()
    {
        if (m_Mapper) {
            delete m_Mapper;
            m_Mapper = 0;
        }
        m_Mapper = new QDataWidgetMapper(q);
        m_Mapper->setModel(m_PatientModel);
        m_Mapper->addMapping(ui->birthName, PatientModel::BirthName, "text");
        m_Mapper->addMapping(ui->secondName, PatientModel::SecondName, "text");
        m_Mapper->addMapping(ui->surname, PatientModel::Surname, "text");
        m_Mapper->addMapping(ui->genderCombo, PatientModel::Gender);
        m_Mapper->addMapping(ui->dob, PatientModel::DateOfBirth);
        m_Mapper->addMapping(ui->street, PatientModel::Street, "plainText");
        m_Mapper->addMapping(ui->city, PatientModel::City, "text");
        m_Mapper->addMapping(ui->zipcode, PatientModel::ZipCode, "text");
        m_Mapper->addMapping(ui->country, PatientModel::Country, "text");
        // add photo
        m_Mapper->toFirst();
    }

public Q_SLOTS:

    void on_prevButton_clicked()
    {
        m_Mapper->toPrevious();
    }

    void on_nextButton_clicked()
    {
        m_Mapper->toNext();
    }


public:
    Ui::IdentityWidget *ui;
    QDataWidgetMapper *m_Mapper;
    Patients::PatientModel *m_PatientModel;

private:
    IdentityWidget *q;
};
}  // End namespace Internal
}  // End namespace Patients


IdentityWidget::IdentityWidget(QWidget *parent) :
    QWidget(parent),
    d(new Internal::IdentityWidgetPrivate(this))
{
}

IdentityWidget::~IdentityWidget()
{
    delete d;
}

void IdentityWidget::setCurrentPatientModel(Patients::PatientModel *model)
{
    d->m_PatientModel = model;
    d->createMapper();
//    d->ui->numberOfPatients->setText(QString::number(model->rowCount()));
}

void IdentityWidget::setCurrentIndex(const QModelIndex &patientIndex)
{
    qWarning() << patientIndex.data();
    d->m_Mapper->setCurrentModelIndex(patientIndex);
}

void IdentityWidget::setCurrentPatient(const QString &uuid)
{

}

void IdentityWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

#include "identitywidget.moc"
