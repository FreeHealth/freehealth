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
#include "patientbar.h"
#include "patientmodel.h"
#include "constants_settings.h"
#include "ui_patientbar.h"

#include <coreplugin/ipatient.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <QDataWidgetMapper>
#include <QIcon>
#include <QPainter>

#include <QDebug>

using namespace Patients;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace Patients {
namespace Internal {
class PatientBarPrivate
{
public:
    PatientBarPrivate() :
            ui(new Ui::PatientBar),
            m_Model(0),
            m_Index(0)
    {
    }

    ~PatientBarPrivate()
    {
        delete ui;
    }

    void setUi()
    {
        Q_ASSERT(m_Model);
        QIcon icon = m_Model->index(m_Index->row(), Core::IPatient::IconizedGender).data().value<QIcon>();
        ui->names->setText(m_Model->index(m_Index->row(), Core::IPatient::FullName).data().toString());
        ui->age->setText(m_Model->index(m_Index->row(), Core::IPatient::Age).data().toString());
        ui->gender->setPixmap(icon.pixmap(QSize(16,16)));
        QPixmap photo = m_Model->index(m_Index->row(), Core::IPatient::Photo).data().value<QPixmap>();
        photo = photo.scaled(QSize(32,32), Qt::KeepAspectRatio);
        ui->photo->setPixmap(photo);
    }

public:
    Ui::PatientBar *ui;
    PatientModel *m_Model;
    QPersistentModelIndex *m_Index;
};
}
}

PatientBar::PatientBar(QWidget *parent) :
    QWidget(parent),
    d(new Internal::PatientBarPrivate)
{
    d->ui->setupUi(this);
    if (!PatientModel::activeModel()) {
        PatientModel::setActiveModel(new PatientModel(qApp));
    }
    setPatientModel(PatientModel::activeModel());
}

PatientBar::~PatientBar()
{
}

void PatientBar::setPatientModel(PatientModel *model)
{
    d->m_Model = model;
}

void PatientBar::setCurrentIndex(const QModelIndex &index)
{
    if (d->m_Index)
        delete d->m_Index;
    d->m_Index = new QPersistentModelIndex(index);
    d->setUi();
}

void PatientBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QRect rect = this->rect();

    QColor background = QColor(settings()->value(Constants::S_PATIENTBARCOLOR, Qt::white).toString());
    background.setAlpha(50);

    QLinearGradient gr(QPoint(rect.center().x(), 0), QPoint(rect.center().x(), rect.bottom()));
    gr.setColorAt(0, Qt::white);
    gr.setColorAt(0.3, QColor(250, 250, 250));
    gr.setColorAt(0.7, QColor(230, 230, 230));

    p.fillRect(rect, gr);
    p.fillRect(rect, background);

    p.setPen(QColor(200, 200, 200));
    p.drawLine(rect.topLeft(), rect.topRight());
    p.setPen(QColor(150, 160, 200));
    p.drawLine(rect.bottomLeft(), rect.bottomRight());
}
