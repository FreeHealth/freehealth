/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Patients::PatientBar
  \brief The Patient bar is the bar surrounding all view when a patient is selected.
  It contains:
  - a brief resume of the patient's identity
  - a specific place where PatientsActions are presented
  \todo The PatientBar is created by the MainWindow and added to Modes manually. Make the patientBar available for everyone in IPatient ?
  \sa Patients::PatientAction
*/

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
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }

namespace Patients {
namespace Internal {
class PatientBarPrivate
{
public:
    PatientBarPrivate(PatientBar *parent) :
        ui(new Ui::PatientBar),
        m_Model(0),
        m_Mapper(0),
        m_Index(0),
        q(parent)
    {
    }

    ~PatientBarPrivate()
    {
        delete ui;
    }

    void setUi()
    {
        Q_ASSERT(m_Model);
        if (m_Mapper)
            return;
        m_Mapper = new QDataWidgetMapper(q);
        m_Mapper->setModel(m_Model);
        m_Mapper->addMapping(ui->names, Core::IPatient::FullName, "text");
        m_Mapper->addMapping(ui->age, Core::IPatient::Age, "text");
        m_Mapper->addMapping(ui->gender, Core::IPatient::GenderPixmap, "pixmap");
        m_Mapper->addMapping(ui->photo, Core::IPatient::Photo, "pixmap");
//        QIcon icon = m_Model->index(m_Index->row(), Core::IPatient::IconizedGender).data().value<QIcon>();
//        ui->names->setText(m_Model->index(m_Index->row(), Core::IPatient::FullName).data().toString());
//        ui->age->setText(m_Model->index(m_Index->row(), Core::IPatient::Age).data().toString());
//        ui->gender->setPixmap(icon.pixmap(QSize(16,16)));
//        QPixmap photo = m_Model->index(m_Index->row(), Core::IPatient::Photo).data().value<QPixmap>();
//        if (!photo.isNull())
//            photo = photo.scaled(QSize(64,64), Qt::KeepAspectRatio);
//        ui->photo->setPixmap(photo);
    }

public:
    Ui::PatientBar *ui;
    PatientModel *m_Model;
    QDataWidgetMapper *m_Mapper;
    QPersistentModelIndex *m_Index;

private:
    PatientBar *q;
};
}
}


PatientBar *PatientBar::m_Instance = 0;
PatientBar *PatientBar::instance(QWidget *parent)
{
    if (!m_Instance)
        m_Instance = new PatientBar(parent);
    return m_Instance;
}

PatientBar::PatientBar(QWidget *parent) :
    QWidget(parent),
    d(new Internal::PatientBarPrivate(this))
{
    d->ui->setupUi(this);
    if (!PatientModel::activeModel()) {
        PatientModel::setActiveModel(new PatientModel(qApp));
    }
    setPatientModel(PatientModel::activeModel());
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
//    connect(patient(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(patientDataChanged(QModelIndex, QModelIndex)));
}

PatientBar::~PatientBar()
{
}

void PatientBar::setPatientModel(PatientModel *model)
{
    d->m_Model = model;
    d->setUi();
    d->m_Mapper->setModel(model);
}

void PatientBar::setCurrentIndex(const QModelIndex &index)
{
    if (d->m_Index)
        delete d->m_Index;
    d->m_Index = new QPersistentModelIndex(index);
//    d->setUi();
    d->m_Mapper->setCurrentModelIndex(index);
}

void PatientBar::onCurrentPatientChanged()
{
    setCurrentIndex(d->m_Model->currentPatient());
}

static bool indexesIncludeColumn(const QModelIndex &left, const QModelIndex &right, int column)
{
    if (left.column()==column || right.column()==column) {
        return true;
    }
    if (left.column() < column)
        return false;
    if (right.column() > column)
        return false;
    return true;
}

//void PatientBar::patientDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
//{
//    if (indexesIncludeColumn(topLeft, bottomRight, Core::IPatient::DateOfBirth)) {

//    }
//}

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
