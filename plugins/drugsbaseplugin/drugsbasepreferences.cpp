/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugsbasepreferences.h"

#include <drugsbaseplugin/constants.h>

#include <drugsbaseplugin/drugsdata.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/drugsbase.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

using namespace DrugsDB;
using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  DrugsBaseOptionPage  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsBaseOptionPage::DrugsBaseOptionPage(QObject *parent) :
        IOptionPage(page), m_Widget(0)
{
    setObjectName("DrugsBaseOptionPage");
}

DrugsBaseOptionPage::~DrugsBaseOptionPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DrugsBaseOptionPage::id() const { return objectName(); }
QString DrugsBaseOptionPage::name() const { return tr("Drugs Database"); }
QString DrugsBaseOptionPage::category() const { return tkTr(Trans::Constants::DRUGS); }

void DrugsBaseOptionPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
}

void DrugsBaseOptionPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void DrugsBaseOptionPage::finish() { delete m_Widget; }

void DrugsBaseOptionPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Constants::S_LEVELOFWARNING, 0);
    defaultvalues.insert(Constants::S_SHOWICONSINPRESCRIPTION,true);
    defaultvalues.insert(Constants::S_ALD_PRE_HTML, 0);
    defaultvalues.insert(Constants::S_ALD_POST_HTML, 0);
    defaultvalues.insert(Constants:, 0);
    defaultvalues.insert(Constants::S_LEVELOFWARNING, 0);
    defaultvalues.insert(Constants::S_LEVELOFWARNING, 0);
    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *DrugsBaseOptionPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugsBaseOptionWidget(parent);
    return m_Widget;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  DrugsBaseOptionWidget  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsBaseOptionWidget::DrugsBaseOptionWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    // feed with actual values
}

void DrugsBaseOptionWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

}

void DrugsBaseOptionWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("DrugsBaseOptionWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugsBaseOptionWidget"));
    s->setValue(MFDRUGS_SETTING_CONFIGURED, true);
    s->setValue(MFDRUGS_SETTING_VIEWFONT , QFont());
    s->setValue(MFDRUGS_SETTING_VIEWFONTSIZE, QFont().pointSize());
    s->setValue(MFDRUGS_SETTING_HISTORYSIZE, 20);
    s->setValue(MFDRUGS_SETTING_DRUGHISTORY, QVariant());
    s->setValue(MFDRUGS_SETTING_LEVELOFWARNING , 0);
    s->setValue(MFDRUGS_SETTING_SHOWICONSINPRESCRIPTION , true);

    s->setValue(MFDRUGS_SETTING_DRUGFONT , QFont().toString());
    s->setValue(MFDRUGS_SETTING_PRESCRIPTIONFONT , QFont().toString());
    s->sync();
}

void DrugsViewWidget::changeEvent(QEvent *e)
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
