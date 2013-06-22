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
#include "edrcpreferences.h"
#include "ui_edrcpreferences.h"

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

using namespace eDRC;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

/* ----------------------  Preferences Widget ---------------------- */

/*! Creates a new preferences widget with a given parent. */
EditorPreferencesWidget::EditorPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditorPreferencesWidget)
{
    ui->setupUi(this);
}

EditorPreferencesWidget::~EditorPreferencesWidget()
{
    delete ui;
}

/*! Sets data of a changed data model to the ui's widgets. */
void EditorPreferencesWidget::setDataToUi()
{
}

/*! \sa IOptionsPage::matches() */
QString EditorPreferencesWidget::searchKeywords() const
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
void EditorPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    // if no sets given as param, take default interface
    Core::ISettings *s = sets? sets : settings();
}

/*! Writes the default settings to the data model. */
void EditorPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
    //    LOG_FOR(tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("EditorPreferencesWidget"));
}

/*! Retranslates the ui widgets to the changed language. */
void EditorPreferencesWidget::retranslateUi()
{
}

void EditorPreferencesWidget::changeEvent(QEvent *e)
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
 * \class Editor::EditorPreferencesPage
 * \brief Generic FreeMedForms preferences page for Editor plugin.
 *
 *  The page is listed in the settings dialog under the category returned by \sa category().
 */

/*
 * All you have to do is to load this preferences page in the constructor in the Editor plugin.
 * Do this like this:
 * \code
 * EditorPlugin::EditorPlugin():
 *     ExtensionSystem::IPlugin()
 * {
 *     _prefPage = new EditorPreferencesPage(this);
 *     addObject(_prefPage);
 * }
 * \endcode
 */

/*! Creates a new preferences page with a given parent. */
EditorPreferencesPage::EditorPreferencesPage(QObject *parent) :
    IOptionsPage(parent),
    m_Widget(0)
{
    setObjectName("EditorPreferencesPage");
}

EditorPreferencesPage::~EditorPreferencesPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

/*! Returns the id if the preferences page. */
QString EditorPreferencesPage::id() const
{
    return objectName();
}

/*! Returns the (translated) name of the preferences page. */
QString EditorPreferencesPage::displayName() const
{
    return tr("General");
}

/*! Returns the (translated) category of the preferences page. */
QString EditorPreferencesPage::category() const
{
    return tr("EDITOR");
}

/*! Returns the (translated) title of the preferences page. */
QString EditorPreferencesPage::title() const
{
    return tr("%PluginName");
}

/*! Returns the sorting order (pages are sorted starting from 0). */
int EditorPreferencesPage::sortIndex() const
{
    return 0;
}

/*! Resets the whole preferences page to the default settings of the settings data model. */
void EditorPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

/*! Overridden function that apllies pending changes to the data model without closing the dialog. */
void EditorPreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void EditorPreferencesPage::finish()
{
    delete m_Widget;
}

/*! \brief Checks if the entered settings are valid.
 *
 * Overloads the interface method. For each empty value the default settings value is written.
 */
void EditorPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    //    defaultvalues.insert(%PluginName:c%::Constants::FOO_SETTING_KEY, %PluginName:c%::Constants::FOO_SETTING_VALUE);
    
    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

bool EditorPreferencesPage::matches(const QString &searchKeyWord) const
{
    return m_searchKeywords.contains(searchKeyWord, Qt::CaseInsensitive);
}

/*! Creates the settings page */
QWidget *EditorPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new EditorPreferencesWidget(parent);
    if (m_searchKeywords.isEmpty())
        m_searchKeywords = m_Widget->searchKeywords();
    return m_Widget;
}

