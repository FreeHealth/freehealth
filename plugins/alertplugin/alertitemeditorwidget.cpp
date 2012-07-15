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

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QTextDocument>

#include "ui_alertitemeditorwidget.h"

#include <QDebug>

using namespace Alert;
using namespace Trans::ConstantTranslations;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}

// TODO: manage multiple relations
// TODO: manage multiple timings
// TODO: manage scripts

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

    bool hideTypeTabRequested()
    {
        return (ui->priority->isHidden() && ui->viewType->isHidden() &&
                ui->contentType->isHidden() && ui->overrideRequiresUserComment->isHidden());
    }

    void manageTabWidgeVisibility()
    {
        if (ui->tabWidget->count() == 0)
            ui->tabWidget->hide();
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

    // BlockingAlert = 0,
    // NonBlockingAlert,
    d->ui->viewType->addItem(tkTr(Trans::Constants::BLOCKING_ALERT));
    d->ui->viewType->addItem(tkTr(Trans::Constants::NON_BLOCKING_ALERT));

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
//    qWarning() << "~AlertItemEditorWidget()";
    if (d)
        delete d;
    d = 0;
}

void AlertItemEditorWidget::clearUi()
{
    d->ui->alertLabel->clear();
    d->ui->alertCategory->clear();
    d->ui->alertDecsr->clear();
    d->ui->iconLineEdit->clear();
    d->ui->contentType->setCurrentIndex(-1);
    d->ui->viewType->setCurrentIndex(-1);
    d->ui->priority->setCurrentIndex(-1);
    d->ui->overrideRequiresUserComment->setChecked(false);
    d->ui->relatedTo->setCurrentIndex(-1);
    d->ui->css->clear();
    d->ui->xml->clear();
    d->ui->tabWidget->setCurrentIndex(0);
    d->ui->timingEditor->clear();
    d->ui->scriptEditor->clear();
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
    if (d->_item.viewType()==AlertItem::BlockingAlert)
        d->ui->viewType->setCurrentIndex(0);
    else if (d->_item.viewType()==AlertItem::NonBlockingAlert)
        d->ui->viewType->setCurrentIndex(1);
    else
        d->ui->viewType->setCurrentIndex(-1);

    d->ui->contentType->setCurrentIndex(d->_item.contentType());
    d->ui->priority->setCurrentIndex(d->_item.priority());
    d->ui->overrideRequiresUserComment->setChecked(d->_item.isOverrideRequiresUserComment());
    d->ui->iconLineEdit->setText(d->_item.themedIcon());

    // Timing
    d->ui->timingEditor->setAlertItem(item);

    // Related to
    // TODO: manage more than one relation
    if (d->_item.relations().count() > 0) {
        const AlertRelation &rel = d->_item.relationAt(0);
        switch (rel.relatedTo()) {
        case AlertRelation::RelatedToPatient: d->ui->relatedTo->setCurrentIndex(0); break;
        case AlertRelation::RelatedToAllPatients: d->ui->relatedTo->setCurrentIndex(1); break;
        case AlertRelation::RelatedToUser: d->ui->relatedTo->setCurrentIndex(2); break;
        case AlertRelation::RelatedToApplication: d->ui->relatedTo->setCurrentIndex(3); break;
        default: break;
        }
    }

    // Scripts
    d->ui->scriptEditor->setAlertItem(d->_item);

    // Set focus
    d->ui->alertLabel->setFocus();
}

void AlertItemEditorWidget::reset()
{
    setAlertItem(d->_item);
}

void AlertItemEditorWidget::setLabelVisible(bool visible)
{
    d->ui->alertLabel->setVisible(visible);
    d->ui->alertLabel_Label->setVisible(visible);
}

void AlertItemEditorWidget::setCategoryVisible(bool visible)
{
    d->ui->alertCategory->setVisible(visible);
    d->ui->alertCategory_Label->setVisible(visible);
}

void AlertItemEditorWidget::setDescriptionVisible(bool visible)
{
    d->ui->alertDecsr->setVisible(visible);
    d->ui->alertDescription_Label->setVisible(visible);
}

void AlertItemEditorWidget::setIconVisible(bool visible)
{
    d->ui->iconLineEdit->setVisible(visible);
    d->ui->iconLabel->setVisible(visible);
}

void AlertItemEditorWidget::setRelationVisible(bool visible)
{
    d->ui->relatedTo->setVisible(visible);
    d->ui->alertRelated_Label->setVisible(visible);
}

void AlertItemEditorWidget::setViewTypeVisible(bool visible)
{
    d->ui->viewType->setVisible(visible);
    d->ui->viewType_Label->setVisible(visible);
    if (d->hideTypeTabRequested())
        hideTypeTab();
}

void AlertItemEditorWidget::setContentTypeVisible(bool visible)
{
    d->ui->contentType->setVisible(visible);
    d->ui->contentType_Label->setVisible(visible);
    if (d->hideTypeTabRequested())
        hideTypeTab();
}

void AlertItemEditorWidget::setPriorityVisible(bool visible)
{
    d->ui->priority->setVisible(visible);
    d->ui->priority_Label->setVisible(visible);
    if (d->hideTypeTabRequested())
        hideTypeTab();
}

void AlertItemEditorWidget::setOverridingCommentVisible(bool visible)
{
    d->ui->overrideRequiresUserComment->setVisible(visible);
    d->ui->override_Label->setVisible(visible);
    if (d->hideTypeTabRequested())
        hideTypeTab();
}

void AlertItemEditorWidget::hideTypeTab()
{
    int id = d->ui->tabWidget->indexOf(d->ui->tab_type);
    d->ui->tabWidget->removeTab(id);
    d->manageTabWidgeVisibility();
}

void AlertItemEditorWidget::hideTimingTab()
{
    int id = d->ui->tabWidget->indexOf(d->ui->tab_timing);
    d->ui->tabWidget->removeTab(id);
    d->manageTabWidgeVisibility();
}

void AlertItemEditorWidget::hideStyleSheetTab()
{
    int id = d->ui->tabWidget->indexOf(d->ui->tab_css);
    d->ui->tabWidget->removeTab(id);
    d->manageTabWidgeVisibility();
}

void AlertItemEditorWidget::hideExtraXmlTab()
{
    int id = d->ui->tabWidget->indexOf(d->ui->tab_xml);
    d->ui->tabWidget->removeTab(id);
    d->manageTabWidgeVisibility();
}

void AlertItemEditorWidget::hideScriptsTab()
{
    int id = d->ui->tabWidget->indexOf(d->ui->tab_scripts);
    d->ui->tabWidget->removeTab(id);
    d->manageTabWidgeVisibility();
}

bool AlertItemEditorWidget::submit(AlertItem &item)
{
    // Description
    // remove all multi-lingual values
    item.removeAllLanguages();
    item.setLabel(d->ui->alertLabel->text());
    item.setCategory(d->ui->alertCategory->text());
    item.setDescription(d->ui->alertDecsr->toHtml());
    item.setThemedIcon(d->ui->iconLineEdit->text());

    // Types
    if (d->ui->viewType->currentIndex() == 0)
        item.setViewType(AlertItem::BlockingAlert);
    else
        item.setViewType(AlertItem::NonBlockingAlert);
    item.setContentType(AlertItem::ContentType(d->ui->contentType->currentIndex()));
    item.setPriority(AlertItem::Priority(d->ui->priority->currentIndex()));
    item.setOverrideRequiresUserComment(d->ui->overrideRequiresUserComment->isChecked());

    // Timing
    item.clearTimings();
    d->ui->timingEditor->submit(item);

    // Related to
    // TODO: manage more than one relation
    item.clearRelations();
    AlertRelation rel;
    switch (d->ui->relatedTo->currentIndex()) {
    case 0:
        rel.setRelatedTo(AlertRelation::RelatedToPatient);
        if (patient())
            rel.setRelatedToUid(patient()->uuid());
        else
            rel.setRelatedToUid("patient1");
        break;
    case 1: rel.setRelatedTo(AlertRelation::RelatedToAllPatients); break;
    case 2:
        rel.setRelatedTo(AlertRelation::RelatedToUser);
        if (user())
            rel.setRelatedToUid(user()->uuid());
        else
            rel.setRelatedToUid("user1");
        break;
    case 3:
        rel.setRelatedTo(AlertRelation::RelatedToApplication);
        rel.setRelatedToUid(qApp->applicationName().toLower());
        break;
    }
    item.addRelation(rel);

    // Scripts
    d->ui->scriptEditor->submit();
    item.clearScripts();
    item.setScripts(d->ui->scriptEditor->scripts());

    return true;
}

