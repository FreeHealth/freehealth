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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "alertitemeditorwidget.h"
#include "alertitem.h"

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QTextDocument>

#include "ui_alertitemeditorwidget.h"

#include <QDebug>

using namespace Alert;
using namespace Trans::ConstantTranslations;

// TODO: manage multiple relations
// TODO: manage multiple timings

namespace Alert {
namespace Internal {
class AlertItemEditorWidgetPrivate
{
public:
    AlertItemEditorWidgetPrivate() :
        ui(new Ui::AlertItemEditorWidget)
    {}

    ~AlertItemEditorWidgetPrivate()
    {
        delete ui;
    }

public:
    Ui::AlertItemEditorWidget *ui;
    AlertItem _item;
};
}  // namespace Internal
}  // namespace Alert

AlertItemEditorWidget::AlertItemEditorWidget(QWidget *parent) :
    QWidget(parent),
    d(new Internal::AlertItemEditorWidgetPrivate)
{
    d->ui->setupUi(this);

    // set up combo
    d->ui->priority->addItem(Utils::firstLetterUpperCase(tkTr(Trans::Constants::HIGH)));
    d->ui->priority->addItem(Utils::firstLetterUpperCase(tkTr(Trans::Constants::MEDIUM)));
    d->ui->priority->addItem(Utils::firstLetterUpperCase(tkTr(Trans::Constants::LOW)));

    // DynamicAlert = 0,
    // StaticAlert,
    d->ui->viewType->addItem(tr("Dynamic alert"));
    d->ui->viewType->addItem(tr("Static alert"));

    // ApplicationNotification = 0,
    // PatientCondition,
    // UserNotification
    d->ui->contentType->addItem(tr("Application notification"));
    d->ui->contentType->addItem(tr("Patient bar notification"));
    d->ui->contentType->addItem(tr("Status bar notification"));

    // RelatedToPatient = 0,
    // RelatedToFamily,
    // RelatedToAllPatients,
    // RelatedToUser,
    // RelatedToUserGroup,
    // RelatedToApplication
    d->ui->relatedTo->addItem(Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_CURRENT_PATIENT)));
//    d->ui->relatedTo->addItem(tkTr(Trans::Constants::RELATED_TO_PATIENT_FAMILY_1).arg(""));
    d->ui->relatedTo->addItem(Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_ALL_PATIENTS)));
    d->ui->relatedTo->addItem(Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_CURRENT_USER)));
//    d->ui->relatedTo->addItem(tkTr(Trans::Constants::RELATED_TO_USER_GROUP_1));
    d->ui->relatedTo->addItem(Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_APPLICATION)));

    clearUi();
}

AlertItemEditorWidget::~AlertItemEditorWidget()
{
    qWarning() << "~AlertItemEditorWidget()";
    if (d)
        delete d;
    d = 0;
}

void AlertItemEditorWidget::clearUi()
{
    d->ui->alertLabel->clear();
    d->ui->alertCategory->clear();
    d->ui->alertDecsr->clear();
    d->ui->contentType->setCurrentIndex(-1);
    d->ui->viewType->setCurrentIndex(-1);
    d->ui->priority->setCurrentIndex(-1);
    d->ui->overrideRequiresUserComment->setChecked(false);
    d->ui->relatedTo->setCurrentIndex(-1);
    d->ui->css->clear();
    d->ui->xml->clear();
    d->ui->tabWidget->setCurrentIndex(0);
    d->ui->timingEditor->clear();
}

void AlertItemEditorWidget::setAlertItem(const AlertItem &item)
{
    d->_item = item;
    clearUi();

    // Description
    d->ui->alertLabel->setText(d->_item.label());
    d->ui->alertCategory->setText(d->_item.category());
    if (Qt::mightBeRichText(d->_item.description()))
        d->ui->alertDecsr->setHtml(d->_item.description());
    else
        d->ui->alertDecsr->setPlainText(d->_item.description());

    // Types
    if (d->_item.viewType()==AlertItem::DynamicAlert)
        d->ui->viewType->setCurrentIndex(0);
    else
        d->ui->viewType->setCurrentIndex(1);
    d->ui->contentType->setCurrentIndex(d->_item.contentType());
    d->ui->priority->setCurrentIndex(d->_item.priority());
    d->ui->overrideRequiresUserComment->setChecked(d->_item.isOverrideRequiresUserComment());

    // Timing
    d->ui->timingEditor->setAlertItem(item);

    // Related to
    if (d->_item.relations().count() > 0) {
        const AlertRelation &rel = d->_item.relationAt(0);
        switch (rel.relatedTo()) {
        case AlertRelation::RelatedToPatient: d->ui->relatedTo->setCurrentIndex(0);
        case AlertRelation::RelatedToAllPatients: d->ui->relatedTo->setCurrentIndex(1);
        case AlertRelation::RelatedToUser: d->ui->relatedTo->setCurrentIndex(2);
        case AlertRelation::RelatedToApplication: d->ui->relatedTo->setCurrentIndex(3);
        }
    }
}

bool AlertItemEditorWidget::submit(AlertItem &item)
{
//    return d->_item;
    return true;
}

