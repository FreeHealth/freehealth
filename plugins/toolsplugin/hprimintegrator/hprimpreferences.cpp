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
 *       Eric Maeker <e>                             *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "hprimpreferences.h"
#include "constants.h"
#include "ui_hprimpreferences.h"

#include <listviewplugin/stringlistmodel.h>

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

/* ----------------------  Preferences Widget ---------------------- */

/*! Creates a new preferences widget with a given parent. */
HprimPreferencesWidget::HprimPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HprimPreferencesWidget)
{
    ui->setupUi(this);
    ui->fileManagement->addItem(tr("Definitively remove file from local drive"));
    ui->fileManagement->addItem(tr("Remove file one month after integration"));
    ui->fileManagement->addItem(tr("Copy file in a specific path"));
    Views::StringListModel *model = new Views::StringListModel(this);
    model->setReadOnly(false);
    model->setCheckable(false);
    model->setStringEditable(true);
    ui->items->setModel(model);
    connect(ui->fileManagement, SIGNAL(activated(int)), this, SLOT(onFileManagementChanged(int)));
}

HprimPreferencesWidget::~HprimPreferencesWidget()
{
    delete ui;
}

/*! Sets data of a changed data model to the ui's widgets. */
void HprimPreferencesWidget::setDataToUi()
{
    ui->items->setStringList(settings()->value(Constants::S_FORMITEM_UUIDS));

    // Just in case we change the ordre of the filemangement combo content
    switch (settings()->value(Constants::S_FILE_MANAGEMENT).toInt()) {
    case Constants::RemoveFileDefinitively: ui->fileManagement->setCurrentIndex(0); break;
    case Constants::RemoveFileOneMonthAfterIntegration: ui->fileManagement->setCurrentIndex(1); break;
    case Constants::StoreFileInPath: ui->fileManagement->setCurrentIndex(2); break;
    }
    ui->pathForIntegratedFiles->setPath(settings()->value(Constants::S_FILE_MANAGEMENT_STORING_PATH).toString());
    ui->pathToScan->setPath(settings()->value(Constants::S_PATH_TO_SCAN).toString());
}

/*! \sa IOptionsPage::matches() */
QString HprimPreferencesWidget::searchKeywords() const
{
    return QString::null;
}

/*! Saves the settings in the ui to the settings data model. */
void HprimPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    // if no sets given as param, take default interface
    Core::ISettings *s = sets? sets : settings();
    s->setValue(Constants::S_FORMITEM_UUIDS, ui->items->getStringList());

    // Just in case we change the ordre of the filemangement combo content
    switch (ui->fileManagement->currentIndex()) {
    case 0: settings()->setValue(Constants::S_FILE_MANAGEMENT, Constants::RemoveFileDefinitively); break;
    case 1: settings()->setValue(Constants::S_FILE_MANAGEMENT, Constants::RemoveFileOneMonthAfterIntegration); break;
    case 2: settings()->setValue(Constants::S_FILE_MANAGEMENT, Constants::StoreFileInPath); break;
    }
    settings()->setValue(Constants::S_FILE_MANAGEMENT_STORING_PATH, ui->pathForIntegratedFiles->path());
    settings()->setValue(Constants::S_PATH_TO_SCAN, ui->pathToScan->path());
}

/*! Writes the default settings to the data model. */
void HprimPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
    // LOG_FOR(tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("HprimPreferencesWidget"));
    // s->setValue(Constants::S_, );
}

void HprimPreferencesWidget::onFileManagementChanged(int index)
{
    switch (index) {
    case 0: ui->pathForIntegratedFiles->setEnabled(false); break;
    case 1: ui->pathForIntegratedFiles->setEnabled(false); break;
    case 2: ui->pathForIntegratedFiles->setEnabled(true); break;
    }
}

/*! Retranslates the ui widgets to the changed language. */
void HprimPreferencesWidget::retranslateUi()
{
}

void HprimPreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi();
        break;
    default:
        break;
    }
}

/* ----------------------  Preferences Page ---------------------- */

/*!
 * \class Tools::HprimPreferencesPage
 * \brief Generic FreeMedForms preferences page for Tools plugin.
 *
 *  The page is listed in the settings dialog under the category returned by \sa category().
 */

/*! Creates a new preferences page with a given parent. */
HprimPreferencesPage::HprimPreferencesPage(QObject *parent) :
    IOptionsPage(parent),
    m_Widget(0)
{
    setObjectName("HprimPreferencesPage");
}

HprimPreferencesPage::~HprimPreferencesPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

/*! Returns the id if the preferences page. */
QString HprimPreferencesPage::id() const
{
    return objectName();
}

/*! Returns the (translated) name of the preferences page. */
QString HprimPreferencesPage::displayName() const
{
    return tr("General");
}

/*! Returns the (translated) category of the preferences page. */
QString HprimPreferencesPage::category() const
{
    return tr("Tools");
}

/*! Returns the (translated) title of the preferences page. */
QString HprimPreferencesPage::title() const
{
    return tr("HPRIM file integrator");
}

/*! Returns the sorting order (pages are sorted starting from 0). */
int HprimPreferencesPage::sortIndex() const
{
    return 0;
}

/*! Resets the whole preferences page to the default settings of the settings data model. */
void HprimPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

/*! Overridden function that apllies pending changes to the data model without closing the dialog. */
void HprimPreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void HprimPreferencesPage::finish()
{
    delete m_Widget;
}

/*! \brief Checks if the entered settings are valid.
 *
 * Overloads the interface method. For each empty value the default settings value is written.
 */
void HprimPreferencesPage::checkSettingsValidity()
{
    // QHash<QString, QVariant> defaultvalues;
    // defaultvalues.insert(Constants::FOO_SETTING_KEY, Constants::FOO_SETTING_VALUE);
    
    // foreach(const QString &k, defaultvalues.keys()) {
    //     if (settings()->value(k) == QVariant())
    //         settings()->setValue(k, defaultvalues.value(k));
    // }
    // settings()->sync();
}

bool HprimPreferencesPage::matches(const QString &) const
{
    return false;
}

/*! Creates the settings page */
QWidget *HprimPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new HprimPreferencesWidget(parent);
    return m_Widget;
}



