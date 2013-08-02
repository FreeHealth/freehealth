/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "drugextrapreferences.h"

#include <drugsplugin/constants.h>
#include <drugsbaseplugin/constants_html.h>
#include <drugsbaseplugin/idrug.h>
#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/drugswidget/drugselector.h>

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_menus.h>

#include <texteditorplugin/texteditor.h>

#include <QPixmap>

using namespace DrugsWidget;
using namespace Internal;
using namespace DrugsWidget::Constants;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}

DrugsExtraWidget::DrugsExtraWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    ALDBefore->setTypes(Editor::TextEditor::Simple | Editor::TextEditor::WithIO | Editor::TextEditor::WithTables);
    ALDAfter->setTypes(Editor::TextEditor::Simple | Editor::TextEditor::WithIO | Editor::TextEditor::WithTables);
    setDataToUi();
}

void DrugsExtraWidget::setDataToUi()
{
    Core::ISettings *s = settings();
    hideLabCheck->setChecked(s->value(DrugsDB::Constants::S_HIDELABORATORY).toBool());
    ALDBefore->textEdit()->setHtml(s->value(DrugsDB::Constants::S_ALD_PRE_HTML).toString());
    ALDAfter->textEdit()->setHtml(s->value(DrugsDB::Constants::S_ALD_POST_HTML).toString());
}

void DrugsExtraWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;
    s->setValue(DrugsDB::Constants::S_HIDELABORATORY, hideLabCheck->isChecked());

    s->setValue(DrugsDB::Constants::S_ALD_PRE_HTML, ALDBefore->textEdit()->toHtml());
    s->setValue(DrugsDB::Constants::S_ALD_POST_HTML, ALDAfter->textEdit()->toHtml());
}

void DrugsExtraWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("DrugsExtraWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugsWidget"));
    s->setValue(S_CONFIGURED, true);
    s->setValue(DrugsDB::Constants::S_HIDELABORATORY, false);

    s->setValue(DrugsDB::Constants::S_ALD_PRE_HTML, DrugsDB::Constants::S_DEF_ALD_PRE_HTML);
    s->setValue(DrugsDB::Constants::S_ALD_POST_HTML, DrugsDB::Constants::S_DEF_ALD_POST_HTML);
}

void DrugsExtraWidget::changeEvent(QEvent *e)
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

DrugsExtraOptionsPage::DrugsExtraOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("DrugsExtraOptionsPage");
}

DrugsExtraOptionsPage::~DrugsExtraOptionsPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

QString DrugsExtraOptionsPage::id() const { return objectName(); }
QString DrugsExtraOptionsPage::displayName() const { return tr("Extras"); }
QString DrugsExtraOptionsPage::category() const { return tkTr(Trans::Constants::DRUGS); }
QString DrugsExtraOptionsPage::title() const {return tr("Drug's extra preferences");}
int DrugsExtraOptionsPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_DRUGS + 100;
}

void DrugsExtraOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void DrugsExtraOptionsPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}
void DrugsExtraOptionsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(DrugsDB::Constants::S_HIDELABORATORY, false);
    defaultvalues.insert(DrugsDB::Constants::S_ALD_PRE_HTML, DrugsDB::Constants::S_DEF_ALD_PRE_HTML);
    defaultvalues.insert(DrugsDB::Constants::S_ALD_POST_HTML, DrugsDB::Constants::S_DEF_ALD_POST_HTML);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

void DrugsExtraOptionsPage::finish() { delete m_Widget; }
QString DrugsExtraOptionsPage::helpPage()
{
    QString l = QLocale().name().left(2);
    if (l=="fr")
        return Constants::H_PREFERENCES_EXTRA_FR;
    return Constants::H_PREFERENCES_EXTRA_EN;
}

QWidget *DrugsExtraOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugsExtraWidget(parent);
    return m_Widget;
}
