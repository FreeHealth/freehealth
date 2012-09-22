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
 *       NAME <MAIL@ADDRESS.COM>                                           *
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

#ifdef WITH_ALERTS
#   include "with-alerts/patientbaralertplaceholder.h"
#endif

#include "ui_patientbar.h"

#include <coreplugin/ipatient.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/stylehelper.h>
#include <extensionsystem/pluginmanager.h>

#include <QDataWidgetMapper>
#include <QIcon>
#include <QPainter>

#include <QDebug>

using namespace Patients;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

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
        m_Mapper->addMapping(ui->gender, Core::IPatient::GenderPixmap, "pixmap");
        m_Mapper->addMapping(ui->photo, Core::IPatient::Photo_64x64, "pixmap");
    }

    void clearUi()
    {
        ui->age->clear();
        ui->gender->clear();
        ui->names->clear();
        ui->photo->clear();
    }

    void createAlertManager()
    {
    #ifdef WITH_ALERTS
        LOG_FOR(q, "Creating patient alert placeholder");
        _alertPlaceHolder = new PatientBarAlertPlaceHolder(q);
        pluginManager()->addObject(_alertPlaceHolder);
        ui->alertLayout->addWidget(_alertPlaceHolder->createWidget(q));
    #endif
    }

    void removeAlertManager()
    {
    #ifdef WITH_ALERTS
        pluginManager()->removeObject(_alertPlaceHolder);
    #endif
    }


public:
    Ui::PatientBar *ui;
    PatientModel *m_Model;
    QDataWidgetMapper *m_Mapper;
    QPersistentModelIndex *m_Index;
#ifdef WITH_ALERTS
    PatientBarAlertPlaceHolder *_alertPlaceHolder;
#endif

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
    setObjectName("PatientBar");
    d->ui->setupUi(this);
    if (!PatientModel::activeModel()) {
        PatientModel::setActiveModel(new PatientModel(qApp));
    }
    setPatientModel(PatientModel::activeModel());
    d->createAlertManager();
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
}

PatientBar::~PatientBar()
{
    d->removeAlertManager();
}

void PatientBar::setPatientModel(PatientModel *model)
{
    if (d->m_Model)
        disconnect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(patientDataChanged(QModelIndex, QModelIndex)));
    d->m_Model = model;
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(patientDataChanged(QModelIndex, QModelIndex)));
    d->setUi();
    d->m_Mapper->setModel(model);
}

void PatientBar::setCurrentIndex(const QModelIndex &index)
{
    if (d->m_Index)
        delete d->m_Index;
    d->m_Index = new QPersistentModelIndex(index);
    d->clearUi();
    QModelIndex top = d->m_Model->index(index.row(),0);
    QModelIndex bottom = d->m_Model->index(index.row(), d->m_Model->columnCount() - 1);
    patientDataChanged(top, bottom);
    d->m_Mapper->setCurrentModelIndex(QModelIndex());
    d->m_Mapper->setCurrentModelIndex(index);
}

void PatientBar::onCurrentPatientChanged()
{
    setCurrentIndex(d->m_Model->currentPatient());
}

void PatientBar::patientDataChanged(const QModelIndex &top, const QModelIndex &bottom)
{
    if (IN_RANGE(Core::IPatient::DateOfBirth, top.column(), bottom.column())) {
        QModelIndex dob = d->m_Model->index(d->m_Index->row(), Core::IPatient::DateOfBirth);
        QModelIndex age = d->m_Model->index(d->m_Index->row(), Core::IPatient::Age);
        d->ui->age->setText(d->m_Model->data(age, Qt::DisplayRole).toString());
        d->ui->age->setToolTip(d->m_Model->data(dob, Qt::ToolTipRole).toString());
    }
}

void PatientBar::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QRect rect = this->rect();
    Utils::StyleHelper::horizontalGradient(&painter, rect, rect, true);
    painter.setPen(QColor(200, 200, 200));
    painter.drawLine(rect.topLeft(), rect.topRight());
    painter.setPen(QColor(150, 160, 200));
    painter.drawLine(rect.bottomLeft(), rect.bottomRight());
    QWidget::paintEvent(e);
}

void PatientBar::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        onCurrentPatientChanged();
    }
    QWidget::changeEvent(event);
}
