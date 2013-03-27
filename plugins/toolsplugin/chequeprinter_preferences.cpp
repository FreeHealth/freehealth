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
 *  Main Developers:                                                       *
 *       Eric Maeker <>                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "chequeprinter_preferences.h"
#include "ui_chequeprinter_preferences.h"

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

/* ----------------------  Preferences Widget ---------------------- */

/*! Creates a new preferences widget with a given parent. */
ChequePrinterPreferencesWidget::ChequePrinterPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChequePrinterPreferencesWidget)
{
    ui->setupUi(this);
    setDataToUi();
}

ChequePrinterPreferencesWidget::~ChequePrinterPreferencesWidget()
{
    delete ui;
}

/*! Sets data of a changed data model to the ui's widgets. */
void ChequePrinterPreferencesWidget::setDataToUi()
{
    ui->orderLineEdit->setText(settings()->value(S_ORDER).toString());
    ui->placeLineEdit->setText(settings()->value(S_PLACE).toString());
    ui->defaultValuesTextEdit->setPlainText(settings()->value(S_VALUES).toStringList().join("\n"));
}

/*! \sa IOptionsPage::matches() */
QString ChequePrinterPreferencesWidget::searchKeywords() const
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
void ChequePrinterPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    // if no sets given as param, take default interface
    Core::ISettings *s = sets? sets : settings();
    s->setValue(S_ORDER, ui->orderLineEdit->text());
    s->setValue(S_PLACE, ui->placeLineEdit->text());
    s->setValue(S_VALUES, ui->defaultValuesTextEdit->toPlainText().split("\n"));
}

/*! Writes the default settings to the data model. */
void ChequePrinterPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
    // Nothing to do
    //    LOG_FOR(tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("ChequePrinterPreferencesWidget"));
}

/*! Retranslates the ui widgets to the changed language. */
void ChequePrinterPreferencesWidget::retranslateUi()
{
    ui->retranslateUi(this);
}

void ChequePrinterPreferencesWidget::changeEvent(QEvent *e)
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
 * \class Tools::ChequePrinterPreferencesPage
 * \brief Generic FreeMedForms preferences page for Account plugin.
 *
 *  The page is listed in the settings dialog under the category returned by \sa category().
 */

/*! Creates a new preferences page with a given parent. */
ChequePrinterPreferencesPage::ChequePrinterPreferencesPage(QObject *parent) :
    IOptionsPage(parent),
    m_Widget(0)
{
    setObjectName("ChequePrinterPreferencesPage");
}

ChequePrinterPreferencesPage::~ChequePrinterPreferencesPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

/*! Returns the id if the preferences page. */
QString ChequePrinterPreferencesPage::id() const
{
    return objectName();
}

/*! Returns the (translated) name of the preferences page. */
QString ChequePrinterPreferencesPage::displayName() const
{
    return tr("Print helpers");
}

/*! Returns the (translated) category of the preferences page. */
QString ChequePrinterPreferencesPage::category() const
{
    return tr("Tools");
}

/*! Returns the (translated) title of the preferences page. */
QString ChequePrinterPreferencesPage::title() const
{
    return tr("Cheque printer");
}

/*! Returns the sorting order (pages are sorted starting from 0). */
int ChequePrinterPreferencesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_TOOLS;
}

/*! Resets the whole preferences page to the default settings of the settings data model. */
void ChequePrinterPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

/*! Overridden function that apllies pending changes to the data model without closing the dialog. */
void ChequePrinterPreferencesPage::apply()
{
    if (!m_Widget)
        return;
    m_Widget->saveToSettings(settings());
}

void ChequePrinterPreferencesPage::finish()
{
    delete m_Widget;
}

/*! \brief Checks if the entered settings are valid.
 *
 * Overloads the interface method. For each empty value the default settings value is written.
 */
void ChequePrinterPreferencesPage::checkSettingsValidity()
{
    // Nothing to do
}

bool ChequePrinterPreferencesPage::matches(const QString &searchKeyWord) const
{
    return m_searchKeywords.contains(searchKeyWord, Qt::CaseInsensitive);
}

/*! Creates the settings page */
QWidget *ChequePrinterPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new ChequePrinterPreferencesWidget(parent);
    if (m_searchKeywords.isEmpty())
        m_searchKeywords = m_Widget->searchKeywords();
    return m_Widget;
}



