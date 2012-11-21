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
#include "druguserpreferences.h"

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/drugswidget/drugselector.h>
#include <drugsbaseplugin/idrug.h>

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

#include <texteditorplugin/texteditor.h>

#include <printerplugin/printer.h>

#include <QPixmap>

using namespace DrugsWidget;
using namespace Internal;
using namespace DrugsWidget::Constants;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}

DrugsUserWidget::DrugsUserWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    previewer = Print::Printer::previewer(this);
    userLayout->addWidget(previewer, 0,0);
    setDataToUi();
}

void DrugsUserWidget::setDataToUi()
{
    Core::ISettings *s = settings();
    previewer->setHeaderHtml(s->value(DrugsDB::Constants::S_USERHEADER).toString());
    previewer->setFooterHtml(s->value(DrugsDB::Constants::S_USERFOOTER).toString());
    previewer->setWatermarkHtml(s->value(DrugsDB::Constants::S_WATERMARK_HTML).toString());
    previewer->setWatermarkPresence(s->value(DrugsDB::Constants::S_WATERMARKPRESENCE).toInt());
}

void DrugsUserWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    s->setValue(DrugsDB::Constants::S_USERHEADER, previewer->headerToHtml());
    s->setValue(DrugsDB::Constants::S_USERFOOTER, previewer->footerToHtml());
    s->setValue(DrugsDB::Constants::S_WATERMARKPRESENCE, previewer->watermarkPresence());
    s->setValue(DrugsDB::Constants::S_WATERMARK_HTML, previewer->watermarkToHtml());
}

void DrugsUserWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("DrugsUserWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugsWidget"));
    s->setValue(S_CONFIGURED, true);
    s->setValue(DrugsDB::Constants::S_WATERMARKPRESENCE, Print::Printer::DuplicatesOnly);
    s->setValue(DrugsDB::Constants::S_WATERMARKALIGNEMENT, Qt::AlignCenter);
    s->setValue(DrugsDB::Constants::S_WATERMARK_HTML, DrugsDB::Constants::S_DEF_WATEMARKHTML);
    s->setValue(DrugsDB::Constants::S_USERHEADER, DrugsDB::Constants::S_DEF_USERHEADER);
    s->setValue(DrugsDB::Constants::S_USERFOOTER, "");
}

void DrugsUserWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

DrugsUserOptionsPage::DrugsUserOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("DrugsUserOptionsPage");
    setId(objectName());
}

DrugsUserOptionsPage::~DrugsUserOptionsPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

QString DrugsUserOptionsPage::displayName() const { return tr("User's documents"); }
QString DrugsUserOptionsPage::category() const { return tkTr(Trans::Constants::DRUGS); }
int DrugsUserOptionsPage::sortIndex() const {return 50;}

void DrugsUserOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void DrugsUserOptionsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void DrugsUserOptionsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(DrugsDB::Constants::S_WATERMARKPRESENCE, Print::Printer::DuplicatesOnly);
    defaultvalues.insert(DrugsDB::Constants::S_WATERMARKALIGNEMENT, Qt::AlignCenter);
    defaultvalues.insert(DrugsDB::Constants::S_WATERMARK_HTML, DrugsDB::Constants::S_DEF_WATEMARKHTML);
    defaultvalues.insert(DrugsDB::Constants::S_USERHEADER, DrugsDB::Constants::S_DEF_USERHEADER);
    defaultvalues.insert(DrugsDB::Constants::S_USERFOOTER, QVariant());

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

void DrugsUserOptionsPage::finish() { delete m_Widget; }
QString DrugsUserOptionsPage::helpPage()
{
    QString l = QLocale().name().left(2);
    if (l=="fr")
        return Constants::H_PREFERENCES_USER_FR;
    return Constants::H_PREFERENCES_USER_EN;
}

QWidget *DrugsUserOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugsUserWidget(parent);
    return m_Widget;
}

