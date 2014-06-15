/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "pmhpreferencespage.h"
#include "constants.h"
#include "pmhcore.h"
#include "pmhcategorymodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <QFontDialog>

using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline PmhCore *pmhCore() {return PmhCore::instance();}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  PmhPreferencesPages  ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
PmhPreferencesPage::PmhPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("PmhPreferencesPages");
}

PmhPreferencesPage::~PmhPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString PmhPreferencesPage::id() const { return objectName(); }
QString PmhPreferencesPage::displayName() const { return tkTr(Trans::Constants::PASTMEDICALHISTORY); }
QString PmhPreferencesPage::category() const { return tkTr(Trans::Constants::PMHX); }
QString PmhPreferencesPage::title() const {return tkTr(Trans::Constants::PMHX_PREFERENCES);}
int PmhPreferencesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_PMHX + 10;
}

void PmhPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void PmhPreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void PmhPreferencesPage::finish() { delete m_Widget; }

void PmhPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    QFont bold;
    bold.setBold(true);
    defaultvalues.insert(Constants::S_FONT_CATEGORIES, bold);
    defaultvalues.insert(Constants::S_FONT_PMH, bold);
    defaultvalues.insert(Constants::S_BACKGROUND_CATEGORIES, "white");
    defaultvalues.insert(Constants::S_BACKGROUND_PMH, "white");
    defaultvalues.insert(Constants::S_FOREGROUND_CATEGORIES, "darkblue");
    defaultvalues.insert(Constants::S_FOREGROUND_PMH, "black");

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
}

QWidget *PmhPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new PmhPreferencesWidget(parent);
    return m_Widget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  PmhPreferencesWidget  /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
PmhPreferencesWidget::PmhPreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    setObjectName("PmhPreferencesWidget");
    setupUi(this);
    setDataToUi();
}

void PmhPreferencesWidget::setDataToUi()
{
    categoryBackgroundButton->setColor(QColor(settings()->value(Constants::S_BACKGROUND_CATEGORIES).toString()));
    categoryForegroundButton->setColor(QColor(settings()->value(Constants::S_FOREGROUND_CATEGORIES).toString()));
    pmhBackgroundButton->setColor(QColor(settings()->value(Constants::S_BACKGROUND_PMH).toString()));
    pmhForegroundButton->setColor(QColor(settings()->value(Constants::S_FOREGROUND_PMH).toString()));
    QFont fontCat;
    QFont fontPmh;
    fontCat.fromString(settings()->value(Constants::S_FONT_CATEGORIES).toString());
    fontPmh.fromString(settings()->value(Constants::S_FONT_PMH).toString());
    changeCatFont->setFont(fontCat);
    changePmhFont->setFont(fontPmh);
}

void PmhPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;
    applyToModel();
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Constants::S_FONT_PMH, changePmhFont->font());
    defaultvalues.insert(Constants::S_FONT_CATEGORIES, changeCatFont->font());
    defaultvalues.insert(Constants::S_BACKGROUND_CATEGORIES, categoryBackgroundButton->color().name());
    defaultvalues.insert(Constants::S_FOREGROUND_CATEGORIES, categoryForegroundButton->color().name());
    defaultvalues.insert(Constants::S_BACKGROUND_PMH, pmhBackgroundButton->color().name());
    defaultvalues.insert(Constants::S_FOREGROUND_PMH, pmhForegroundButton->color().name());

    foreach(const QString &k, defaultvalues.keys()) {
        s->setValue(k, defaultvalues.value(k));
    }
}

void PmhPreferencesWidget::applyToModel()
{
    pmhCore()->pmhCategoryModel()->updateFontAndColors();
}

void PmhPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    LOG_FOR("PmhPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("PmhPreferencesWidget"));
    QHash<QString, QVariant> defaultvalues;
    QFont bold;
    bold.setBold(true);
    defaultvalues.insert(Constants::S_FONT_CATEGORIES, bold);
    defaultvalues.insert(Constants::S_FONT_PMH, bold);
    defaultvalues.insert(Constants::S_BACKGROUND_CATEGORIES, "white");
    defaultvalues.insert(Constants::S_BACKGROUND_PMH, "white");
    defaultvalues.insert(Constants::S_FOREGROUND_CATEGORIES, "darkblue");
    defaultvalues.insert(Constants::S_FOREGROUND_PMH, "black");

    foreach(const QString &k, defaultvalues.keys()) {
        settings()->setValue(k, defaultvalues.value(k));
    }
    s->sync();
    applyToModel();
}

void PmhPreferencesWidget::on_changePmhFont_clicked()
{
    // open font dialog
    QFontDialog dlg(this);
    dlg.setCurrentFont(changePmhFont->font());
    dlg.setModal(true);
    if (dlg.exec()==QDialog::Accepted) {
        // set font to pushbutton
        changePmhFont->setFont(dlg.selectedFont());
    }
}

void PmhPreferencesWidget::on_changeCatFont_clicked()
{
    // open font dialog
    QFontDialog dlg(this);
    dlg.setCurrentFont(changeCatFont->font());
    dlg.setModal(true);
    if (dlg.exec()==QDialog::Accepted) {
        // set font to pushbutton
        changeCatFont->setFont(dlg.selectedFont());
    }
}

void PmhPreferencesWidget::changeEvent(QEvent *e)
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

