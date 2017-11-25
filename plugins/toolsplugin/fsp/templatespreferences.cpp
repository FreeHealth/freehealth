/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developpers:                                                     *
 *       Eric Maeker <e>                             *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "templatespreferences.h"
#include "ui_templatespreferences.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

using namespace Templates;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

/* ----------------------  Preferences Widget ---------------------- */

/*! Creates a new preferences widget with a given parent. */
TemplatesPreferencesWidget::TemplatesPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TemplatesPreferencesWidget)
{
    ui->setupUi(this);
}

TemplatesPreferencesWidget::~TemplatesPreferencesWidget()
{
    delete ui;
}

/*! Sets data of a changed data model to the ui's widgets. */
void TemplatesPreferencesWidget::setDataToUi()
{
}

QString TemplatesPreferencesWidget::searchKeywords() const
{
    QString rc;
    // uncomment these lines to add keywords to the search index
    
    //    QTextStream(&rc)
    //            << ui->deviceInfoLabel->text()
    ;
    rc.remove(QLatin1Char('&'));
    return rc;
}

/*! \sa IOptionsPage::matches() */
QString TemplatesPreferencesWidget::searchKeywords() const
{
    QString rc;
    // uncomment this lines to add some keywords from UI elements
    // QTextStream(&rc)
    //        << ui->someLabel->text()
    //        << ui->otherLabel->text();
    
    rc.remove(QLatin1Char('&'));
    return rc;
}

/*! Saves the settings in the ui to the settings data model. */
void TemplatesPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    // if no sets given as param, take default interface
    Core::ISettings *s = sets? sets : settings();
}

/*! Writes the default settings to the data model. */
void TemplatesPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
    //    LOG_FOR(tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("TemplatesPreferencesWidget"));
}

/*! Retranslates the ui widgets to the changed language. */
void TemplatesPreferencesWidget::retranslateUi()
{
}

void TemplatesPreferencesWidget::changeEvent(QEvent *e)
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
 * \class Templates::TemplatesPreferencesPage
 * \brief Generic FreeMedForms preferences page for Templates plugin.
 *
 *  The page is listed in the settings dialog under the category returned by \sa category().
 */

/*
 * All you have to do is to load this preferences page in the constructor in the Templates plugin.
 * Do this like this:
 * \code
 * TemplatesPlugin::TemplatesPlugin():
 *     ExtensionSystem::IPlugin()
 * {
 *     _prefPage = new TemplatesPreferencesPage(this);
 *     addObject(_prefPage);
 * }
 * \endcode
 */

/*! Creates a new preferences page with a given parent. */
TemplatesPreferencesPage::TemplatesPreferencesPage(QObject *parent) :
    IOptionsPage(parent),
    m_Widget(0)
{
    setObjectName("TemplatesPreferencesPage");
}

TemplatesPreferencesPage::~TemplatesPreferencesPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

/*! Returns the id if the preferences page. */
QString TemplatesPreferencesPage::id() const
{
    return objectName();
}

/*! Returns the (translated) name of the preferences page. */
QString TemplatesPreferencesPage::displayName() const
{
    return tr("General");
}

/*! Returns the (translated) category of the preferences page. */
QString TemplatesPreferencesPage::category() const
{
    return tr("TEMPLATES");
}

/*! Returns the (translated) title of the preferences page. */
QString TemplatesPreferencesPage::title() const
{
    return tr("%PluginName");
}

/*! Returns the sorting order (pages are sorted starting from 0). */
int %PluginName:c%PreferencesPage::sortIndex() const
{
    return 0; // Core::Constants::OPTIONINDEX_...;
}

/*! Resets the whole preferences page to the default settings of the settings data model. */
void %PluginName:c%PreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

/*! Overridden function that apllies pending changes to the data model without closing the dialog. */
void %PluginName:c%PreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void %PluginName:c%PreferencesPage::finish()
{
    delete m_Widget;
}

/*! \brief Checks if the entered settings are valid.
 *
 * Overloads the interface method. For each empty value the default settings value is written.
 */
void %PluginName:c%PreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    //    defaultvalues.insert(%PluginName:c%::Constants::FOO_SETTING_KEY, %PluginName:c%::Constants::FOO_SETTING_VALUE);
    
    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

bool %PluginName:c%PreferencesPage::matches(const QString &searchKeyWord) const
{
    return m_searchKeywords.contains(s, Qt::CaseInsensitive);
}

/*! Creates the settings page */
QWidget *%PluginName:c%PreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new %PluginName:c%PreferencesWidget(parent);
    if (m_searchKeywords.isEmpty)
        m_searchKeywords = m_Widget->searchKeywords();
    return m_Widget;
}



