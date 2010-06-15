/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "templatespreferencespages.h"
#include "constants.h"
#include "templatesview.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>

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
    checkSettingsValidity();
}

TemplatesPreferencesPage::~TemplatesPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString TemplatesPreferencesPage::id() const { return objectName(); }
QString TemplatesPreferencesPage::name() const { return tkTr(Trans::Constants::TEMPLATES); }
QString TemplatesPreferencesPage::category() const { return tkTr(Trans::Constants::TEMPLATES); }

void TemplatesPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void TemplatesPreferencesPage::applyChanges()
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
    settings()->sync();
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
    setDatasToUi();
}

void TemplatesPreferencesWidget::setDatasToUi()
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
        settings()->setValue(k, defaultvalues.value(k));
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

