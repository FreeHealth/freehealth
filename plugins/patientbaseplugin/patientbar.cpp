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
/**
 * \class Patients::PatientBar
 * \brief The Patient bar is the bar on the top of all views if a patient is selected.
 *
 * If no patient is selected, the PatientBar is hidden.
 * It contains:
 * - a brief summary of the patient's identity
 * - a place holder at the bottom to add various widgets (like alert place holders)
 * - a specific place where PatientsActions are presented (not yet implemented)
 *
 * The whole application owns a unique instance of the patient bar. This instance is accessible
 * throught the Patients::PatientCore interface.
 *
 * The PatientBar acts like a QDataMapperWidget over the Core::IPatient model.
 * Just ensure that the Core::IPatient sub-class emits the dataChanged()
 * correctly to keep the bar up to date.
 *
 * \sa Patients::PatientAction, Patients::Internal::PatientBarAlertPlaceHolder, Core::IPatient::showPatientBar(), Core::IPatient::hidePatientBar(), Core::IPatient::isPatientBarVisible(), Patients::PatientCore::patientBar()
*/

#include "patientbar.h"
#include "constants_settings.h"

#include "ui_patientbar.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/stylehelper.h>
#include <extensionsystem/pluginmanager.h>

#include <QDataWidgetMapper>
#include <QIcon>
#include <QPainter>
#include <QPointer>
#include <QTimer>

#include <QDebug>

using namespace Patients;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

namespace Patients {
namespace Internal {
class PatientBarPrivate
{
public:
    PatientBarPrivate(PatientBar *parent) :
        ui(new Ui::PatientBar),
        m_Mapper(0),
        m_Index(0),
        m_MessageLabel(0),
        q(parent)
    {
    }

    ~PatientBarPrivate()
    {
        delete ui;
    }

    void clearUi()
    {
        ui->age->clear();
        ui->gender->clear();
        ui->names->clear();
        ui->photo->clear();
    }

    void updateUi()
    {
        ui->age->setText(patient()->data(Core::IPatient::Age).toString());
        QModelIndex dob = patient()->index(patient()->currentPatientIndex().row(), Core::IPatient::DateOfBirth);
        ui->age->setToolTip(patient()->data(dob, Qt::ToolTipRole).toString());
        ui->gender->setPixmap(patient()->data(Core::IPatient::GenderPixmap).value<QPixmap>());
        ui->names->setText(patient()->data(Core::IPatient::FullName).toString());
        updatePatientPhoto();
    }

    void updatePatientPhoto()
    {
        QPixmap photo = patient()->data(Core::IPatient::Photo_64x64).value<QPixmap>();
        if (photo.isNull()) {
            const int gender = patient()->data(Core::IPatient::GenderIndex).toInt();
            photo = theme()->defaultGenderPixmap(gender);
        }
        ui->photo->setPixmap(photo);
    }

public:
    Ui::PatientBar *ui;
    QDataWidgetMapper *m_Mapper;
    QPersistentModelIndex *m_Index;
    QPointer<QLabel> m_MessageLabel;

private:
    PatientBar *q;
};
}  // namespace Internal
}  // namespace Patients

/**
 * ctor of the widget, uses the Core::IPatient has central model.
 * The unique instance is available in Patients::PatientCore::patientBar()
 */
PatientBar::PatientBar(QWidget *parent) :
    Core::IPatientBar(parent),
    d(new Internal::PatientBarPrivate(this))
{
    setObjectName("PatientBar");
    d->ui->setupUi(this);
//    d->setUi();
    connect(patient(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onPatientDataChanged(QModelIndex, QModelIndex)));
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
}

/** dtor */
PatientBar::~PatientBar()
{
}

/** Add a widget at the bottom of the patient bar */
void PatientBar::addBottomWidget(QWidget *widget)
{
    d->ui->bottomLayout->addWidget(widget);
}

void PatientBar::showMessage(const QString &message, int duration_ms, const QString &css)
{
    LOG(message);
    if (d->m_MessageLabel) {
        delete d->m_MessageLabel;
        d->m_MessageLabel = 0;
    }
    d->m_MessageLabel = new QLabel(this);
    d->m_MessageLabel->setText(message);
    d->m_MessageLabel->setStyleSheet(QString("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 %2);")
                                     .arg(this->palette().base().color().lighter(100).name())
                                     .arg(this->palette().base().color().lighter(120).name()));
    if (!css.isEmpty())
        d->m_MessageLabel->setStyleSheet(css);
    d->m_MessageLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->m_MessageLabel->adjustSize();

    QPoint topRight = rect().topRight();
    QSize size = d->m_MessageLabel->rect().size();
    QRect rect(topRight.x() - d->m_MessageLabel->rect().width(), topRight.y() + 2, size.width(), size.height());
    d->m_MessageLabel->setGeometry(rect);
    d->m_MessageLabel->show();
    d->m_MessageLabel->raise();
    QTimer::singleShot(duration_ms, d->m_MessageLabel, SLOT(deleteLater()));
}

/** Set the current patient index when patient changed */
void PatientBar::onCurrentPatientChanged()
{
    d->updateUi();
}

/** Update the view when a data of the patient model is changed */
void PatientBar::onPatientDataChanged(const QModelIndex &top, const QModelIndex &bottom)
{
    // Test identity "strings" and "dates"
    QList<int> test;
    test << Core::IPatient::DateOfBirth
         << Core::IPatient::Age
         << Core::IPatient::FullName
         << Core::IPatient::FullAddress
         << Core::IPatient::GenderIndex
            ;
    foreach(int ref, test) {
        if (IN_RANGE(ref, top.column(), bottom.column())) {
            d->updateUi();
            break;
        }
    }

    // Test photo
    test.clear();
    test << Core::IPatient::Photo_32x32
         << Core::IPatient::Photo_64x64;
    foreach(int ref, test) {
        if (IN_RANGE(ref, top.column(), bottom.column())) {
            d->updatePatientPhoto();
            break;
        }
    }
}

/** Paint the bar using the Utils::StyleHelper */
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
