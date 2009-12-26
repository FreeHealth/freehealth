/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>

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
        IOptionsPage(parent), m_Widget(0) { setObjectName("TemplatesPreferencesPages"); }

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
    defaultvalues.insert(Constants::S_BACKGROUND_CATEGORIES, "white");
    defaultvalues.insert(Constants::S_BACKGROUND_TEMPLATES, "white");
    defaultvalues.insert(Constants::S_FOREGROUND_CATEGORIES, "darkblue");
    defaultvalues.insert(Constants::S_FOREGROUND_TEMPLATES, "black");
    defaultvalues.insert(Constants::S_SPLITTER_SIZES, QVariant());
    defaultvalues.insert(Constants::S_ALWAYSSHOWEXPANDED, true);
    defaultvalues.insert(Constants::S_SAVEDATAWITHOUTPROMPTING, true);

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
    // feed with actual values
    promptBox->setChecked(settings()->value(Constants::S_SAVEDATAWITHOUTPROMPTING).toBool());
    autoExpandBox->setChecked(settings()->value(Constants::S_ALWAYSSHOWEXPANDED).toBool());
}

void TemplatesPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(Constants::S_BACKGROUND_CATEGORIES, "white");
//    defaultvalues.insert(Constants::S_BACKGROUND_TEMPLATES, "white");
//    defaultvalues.insert(Constants::S_FOREGROUND_CATEGORIES, "darkblue");
//    defaultvalues.insert(Constants::S_FOREGROUND_TEMPLATES, "black");
    defaultvalues.insert(Constants::S_ALWAYSSHOWEXPANDED, promptBox->isChecked());
    defaultvalues.insert(Constants::S_SAVEDATAWITHOUTPROMPTING, autoExpandBox->isChecked());

    foreach(const QString &k, defaultvalues.keys()) {
        settings()->setValue(k, defaultvalues.value(k));
    }
}

void TemplatesPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Utils::Log::addMessage("TemplatesPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("TemplatesPreferencesWidget"));
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Constants::S_BACKGROUND_CATEGORIES, "white");
    defaultvalues.insert(Constants::S_BACKGROUND_TEMPLATES, "white");
    defaultvalues.insert(Constants::S_FOREGROUND_CATEGORIES, "darkblue");
    defaultvalues.insert(Constants::S_FOREGROUND_TEMPLATES, "black");
    defaultvalues.insert(Constants::S_SPLITTER_SIZES, QVariant());
    defaultvalues.insert(Constants::S_ALWAYSSHOWEXPANDED, true);
    defaultvalues.insert(Constants::S_SAVEDATAWITHOUTPROMPTING, true);

    foreach(const QString &k, defaultvalues.keys()) {
        settings()->setValue(k, defaultvalues.value(k));
    }
    s->sync();
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

