/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "templatespreferencespages.h"
#include "constants.h"
#include "templatesview.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <QObject>

using namespace Templates;
using namespace Templates::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}


/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  TemplatesPreferencesPages  /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
TemplatesPreferencesPage::TemplatesPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("TemplatesPreferencesPages");
}

TemplatesPreferencesPage::~TemplatesPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString TemplatesPreferencesPage::id() const { return objectName(); }
QString TemplatesPreferencesPage::displayName() const { return tkTr(Trans::Constants::TEMPLATES); }
QString TemplatesPreferencesPage::category() const { return tkTr(Trans::Constants::TEMPLATES); }
QString TemplatesPreferencesPage::title() const {return tr("Templates preferences"); }
int TemplatesPreferencesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_TEMPLATES;
}

void TemplatesPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void TemplatesPreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void TemplatesPreferencesPage::finish() { delete m_Widget; }

void TemplatesPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Constants::S_FONT, QFont());
    defaultvalues.insert(Constants::S_BACKGROUND_CATEGORIES, "white");
    defaultvalues.insert(Constants::S_BACKGROUND_TEMPLATES, "white");
    defaultvalues.insert(Constants::S_FOREGROUND_CATEGORIES, "darkblue");
    defaultvalues.insert(Constants::S_FOREGROUND_TEMPLATES, "black");
    defaultvalues.insert(Constants::S_SPLITTER_SIZES, QVariant());
    defaultvalues.insert(Constants::S_ALWAYSSHOWEXPANDED, true);
    defaultvalues.insert(Constants::S_LOCKCATEGORYVIEW, false);
    defaultvalues.insert(Constants::S_PROMPTFORDELETION, true);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
}

QWidget *TemplatesPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new TemplatesPreferencesWidget(parent);
    return m_Widget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  TemplatesPreferencesWidget  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
TemplatesPreferencesWidget::TemplatesPreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    setDataToUi();
}

void TemplatesPreferencesWidget::setDataToUi()
{
    deletionBox->setChecked(settings()->value(Constants::S_PROMPTFORDELETION).toBool());
    autoExpandBox->setChecked(settings()->value(Constants::S_ALWAYSSHOWEXPANDED).toBool());
    lockViewBox->setChecked(settings()->value(Constants::S_LOCKCATEGORYVIEW).toBool());
    categoryBackgroundButton->setColor(QColor(settings()->value(Constants::S_BACKGROUND_CATEGORIES).toString()));
    templateBackgroundButton->setColor(QColor(settings()->value(Constants::S_BACKGROUND_TEMPLATES).toString()));
    categoryForegroundButton->setColor(QColor(settings()->value(Constants::S_FOREGROUND_CATEGORIES).toString()));
    templateForegroundButton->setColor(QColor(settings()->value(Constants::S_FOREGROUND_TEMPLATES).toString()));
    QFont font;
    font.fromString(settings()->value(Constants::S_FONT).toString());
    fontBox->setCurrentFont(font);
    sizeSpin->setValue(font.pointSize());
}

void TemplatesPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;
    QFont font = fontBox->currentFont();
    font.setPointSize(sizeSpin->value());
    appliFontToViews(font);
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Constants::S_FONT, font);
    defaultvalues.insert(Constants::S_BACKGROUND_CATEGORIES, categoryBackgroundButton->color().name());
    defaultvalues.insert(Constants::S_BACKGROUND_TEMPLATES, templateBackgroundButton->color().name());
    defaultvalues.insert(Constants::S_FOREGROUND_CATEGORIES, categoryForegroundButton->color().name());
    defaultvalues.insert(Constants::S_FOREGROUND_TEMPLATES, templateForegroundButton->color().name());
    defaultvalues.insert(Constants::S_ALWAYSSHOWEXPANDED, autoExpandBox->isChecked());
    defaultvalues.insert(Constants::S_LOCKCATEGORYVIEW, lockViewBox->isChecked());
    defaultvalues.insert(Constants::S_PROMPTFORDELETION, deletionBox->isChecked());

    foreach(const QString &k, defaultvalues.keys()) {
        s->setValue(k, defaultvalues.value(k));
    }
}

void TemplatesPreferencesWidget::appliFontToViews(const QFont &font)
{
    QList<Templates::TemplatesView *> allViews = Core::ICore::instance()->mainWindow()->findChildren<Templates::TemplatesView *>();
    for(int i = 0; i < allViews.count(); ++i) {
        allViews.at(i)->setFont(font);
    }
}

void TemplatesPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Utils::Log::addMessage("TemplatesPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("TemplatesPreferencesWidget"));
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Constants::S_FONT, QFont());
    defaultvalues.insert(Constants::S_BACKGROUND_CATEGORIES, "white");
    defaultvalues.insert(Constants::S_BACKGROUND_TEMPLATES, "white");
    defaultvalues.insert(Constants::S_FOREGROUND_CATEGORIES, "darkblue");
    defaultvalues.insert(Constants::S_FOREGROUND_TEMPLATES, "black");
    defaultvalues.insert(Constants::S_SPLITTER_SIZES, QVariant());
    defaultvalues.insert(Constants::S_ALWAYSSHOWEXPANDED, true);
    defaultvalues.insert(Constants::S_LOCKCATEGORYVIEW, false);
    defaultvalues.insert(Constants::S_PROMPTFORDELETION, true);

    foreach(const QString &k, defaultvalues.keys()) {
        settings()->setValue(k, defaultvalues.value(k));
    }
    s->sync();
    appliFontToViews(QFont());
}

void TemplatesPreferencesWidget::changeEvent(QEvent *e)
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

