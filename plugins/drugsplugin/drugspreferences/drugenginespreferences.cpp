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
#include "drugenginespreferences.h"

#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/idrugengine.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <extensionsystem/pluginmanager.h>

#include "ui_drugenginespreferences.h"

#include <QCheckBox>
#include <QTextBrowser>
#include <QGridLayout>

#include <QDebug>

using namespace DrugsWidget;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

DrugEnginesPreferences::DrugEnginesPreferences(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrugEnginesPreferences)
{
    ui->setupUi(this);
    setDatasToUi();
}

DrugEnginesPreferences::~DrugEnginesPreferences()
{
    delete ui;
}

void DrugEnginesPreferences::setDatasToUi()
{
    // Get all IDrugEngine objects
    QList<DrugsDB::IDrugEngine *> engines = pluginManager()->getObjects<DrugsDB::IDrugEngine>();
    QGridLayout *scrollLayout = qobject_cast<QGridLayout*>(ui->scrollAreaWidgetContents->layout());
    scrollLayout->setSpacing(24);
    for(int i=0; i < engines.count(); ++i) {
        DrugsDB::IDrugEngine *engine = engines.at(i);
        // Create widget
//        QWidget *w = new QWidget(this);
//        QGridLayout *l = new QGridLayout(w);
//        w->setLayout(l);
//        l->setMargin(0);
        // with checkbox
        QCheckBox *box = new QCheckBox(this);
        box->setText(engine->shortName() + ", " + engine->name());
        box->setToolTip(engine->tooltip());
        box->setChecked(engine->isActive());
        box->setIcon(engine->icon());
        // and a small explanation
//        QTextBrowser *browser = new QTextBrowser(w);
//        browser->setText(engines.at(i));
        scrollLayout->addWidget(box, i, 0);
        connect(box, SIGNAL(clicked(bool)), engine, SLOT(setActive(bool)));
    }
    QSpacerItem *s = new QSpacerItem(20,20, QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollLayout->addItem(s, engines.count()+1, 0);
}

void DrugEnginesPreferences::writeDefaultSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    // get default activated drug engines
    QList<DrugsDB::IDrugEngine *> engines = pluginManager()->getObjects<DrugsDB::IDrugEngine>();
    QStringList uids;
    uids << "__";
    for(int i=0; i < engines.count(); ++i) {
        DrugsDB::IDrugEngine *engine = engines.at(i);
        if (engine->isActiveByDefault())
            uids << engine->uid();
    }

    LOG_FOR("DrugEnginesPreferences", "Activating default drug engines: " + uids.join("; "));
    s->setValue(DrugsDB::Constants::S_ACTIVATED_INTERACTION_ENGINES, uids);
}

void DrugEnginesPreferences::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    QList<DrugsDB::IDrugEngine *> engines = pluginManager()->getObjects<DrugsDB::IDrugEngine>();
    QStringList uids;
    uids << "__"; // uids must not be empty is defined by the user
    for(int i=0; i < engines.count(); ++i) {
        DrugsDB::IDrugEngine *engine = engines.at(i);
        if (engine->isActive())
            uids << engine->uid();
    }
    s->setValue(DrugsDB::Constants::S_ACTIVATED_INTERACTION_ENGINES, uids);
}

DrugEnginesPreferencesPage::DrugEnginesPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("DrugEnginesPreferencesPage");
}

DrugEnginesPreferencesPage::~DrugEnginesPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DrugEnginesPreferencesPage::id() const { return objectName(); }
QString DrugEnginesPreferencesPage::name() const { return tr("Interaction engines"); }
QString DrugEnginesPreferencesPage::category() const { return tkTr(Trans::Constants::DRUGS); }
QString DrugEnginesPreferencesPage::title() const {return tr("Interaction engines");}
int DrugEnginesPreferencesPage::sortIndex() const {return 30;}

void DrugEnginesPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void DrugEnginesPreferencesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void DrugEnginesPreferencesPage::checkSettingsValidity()
{
    // get default activated drug engines
    QList<DrugsDB::IDrugEngine *> engines = pluginManager()->getObjects<DrugsDB::IDrugEngine>();
    QStringList uids;
    uids << "__";
    for(int i=0; i < engines.count(); ++i) {
        DrugsDB::IDrugEngine *engine = engines.at(i);
        if (engine->isActiveByDefault())
            uids << engine->uid();
    }

    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(DrugsDB::Constants::S_ACTIVATED_INTERACTION_ENGINES, uids);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k).toString().simplified().isEmpty()) {
            LOG("Activating default drugs engines: " + uids.join("; "));
            settings()->setValue(k, defaultvalues.value(k));
        }
    }
}

void DrugEnginesPreferencesPage::finish() { delete m_Widget; }
QString DrugEnginesPreferencesPage::helpPage()
{
//    QString l = QLocale().name().left(2);
//    if (l=="fr")
//        return Constants::H_PREFERENCES_USER_FR;
//    return Constants::H_PREFERENCES_USER_EN;
    return QString();
}

QWidget *DrugEnginesPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugEnginesPreferences(parent);
    return m_Widget;
}
