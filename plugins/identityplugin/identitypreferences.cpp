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
 *  Main Developers:                                                       *
 *       Eric Maeker <eric.maeker@gmail.com>                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "identitypreferences.h"
#include "ui_identitypreferences.h"

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

using namespace Identity;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

/* ----------------------  Preferences Widget ---------------------- */

/*! Creates a new preferences widget with a given parent. */
IdentityPreferencesWidget::IdentityPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IdentityPreferencesWidget)
{
    ui->setupUi(this);
}

IdentityPreferencesWidget::~IdentityPreferencesWidget()
{
    delete ui;
}

/*! Sets data of a changed data model to the ui's widgets. */
void IdentityPreferencesWidget::setDataToUi()
{
}

/*! \sa IOptionsPage::matches() */
QString IdentityPreferencesWidget::searchKeywords() const
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
void IdentityPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    // if no sets given as param, take default interface
    Core::ISettings *s = sets? sets : settings();
}

/*! Writes the default settings to the data model. */
void IdentityPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
    //    LOG_FOR(tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("IdentityPreferencesWidget"));
}

/*! Retranslates the ui widgets to the changed language. */
void IdentityPreferencesWidget::retranslateUi()
{
}

void IdentityPreferencesWidget::changeEvent(QEvent *e)
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
 * \class IdentityPreferencesPage
 * \brief Generic FreeMedForms preferences page for Identity plugin.
 *
 * A IdentityPreferencesPage creates a page in the FMF settings dialog, it is listed in
 * the category returned by \sa category().
 *
 * All you have to do is to load this preferences page in the constructor if the Identity plugin.
 * Do this like this:
 * \code
 * IdentityPlugin::IdentityPlugin():
 *     ExtensionSystem::IPlugin()
 * {
 *     _prefPage = new IdentityPreferencesPage(this);
 *     addObject(_prefPage);
 * }
 * \endcode
 */

/*! Creates a new preferences page with a given parent. */
IdentityPreferencesPage::IdentityPreferencesPage(QObject *parent) :
    IOptionsPage(parent),
    m_Widget(0)
{
    setObjectName("IdentityPreferencesPage");
}

IdentityPreferencesPage::~IdentityPreferencesPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

/*! Returns the id if the preferences page. */
QString IdentityPreferencesPage::id() const
{
    return objectName();
}

/*! Returns the (translated) name of the preferences page. */
QString IdentityPreferencesPage::name() const
{
    return tr("General");
}

/*! Returns the (translated) category of the preferences page. */
QString IdentityPreferencesPage::category() const
{
    return tr("IDENTITY");
}

/*! Resets the whole preferences page to the default settings of the settings data model. */
void IdentityPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

/*! Overridden function that apllies pending changes to the data model without closing the dialog. */
void IdentityPreferencesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void IdentityPreferencesPage::finish()
{
    delete m_Widget;
}

/*! \brief Checks if the entered settings are valid.
 *
 * Overloads the interface method. For each empty value the default settings value is written.
 */
void IdentityPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    //    defaultvalues.insert(Identity::Constants::FOO_SETTING_KEY, Identity::Constants::FOO_SETTING_VALUE);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

/*! Creates the settings page */
QWidget *IdentityPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new IdentityPreferencesWidget(parent);
    return m_Widget;
}



