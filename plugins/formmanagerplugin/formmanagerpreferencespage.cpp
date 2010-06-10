/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
 ***************************************************************************/
#include "formmanagerpreferencespage.h"
#include "formfilesselectorwidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <QHBoxLayout>

using namespace Form;
using namespace Internal;


static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }


FormManagerPreferencesPage::FormManagerPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{ setObjectName("FormManagerPreferencesPage"); }

FormManagerPreferencesPage::~FormManagerPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString FormManagerPreferencesPage::id() const { return objectName(); }
QString FormManagerPreferencesPage::name() const { return tr("Selector"); }
QString FormManagerPreferencesPage::category() const { return tr("Forms"); }

void FormManagerPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
}

void FormManagerPreferencesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void FormManagerPreferencesPage::finish() { delete m_Widget; }

void FormManagerPreferencesPage::checkSettingsValidity()
{
//    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(Utils::Constants::S_CHECKUPDATE, Utils::UpdateChecker::Check_AtStartup);
//    defaultvalues.insert(Core::Constants::S_USE_EXTERNAL_DATABASE, false);
//
//    foreach(const QString &k, defaultvalues.keys()) {
//        if (settings()->value(k) == QVariant())
//            settings()->setValue(k, defaultvalues.value(k));
//    }
//    settings()->sync();
}

QWidget *FormManagerPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new FormManagerPreferencesWidget(parent);
    return m_Widget;
}


FormManagerPreferencesWidget::FormManagerPreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    m_Selector = new Form::FormFilesSelectorWidget(this);
    QHBoxLayout *l = new QHBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);
    l->addWidget(m_Selector);
}

void FormManagerPreferencesWidget::saveToSettings(Core::ISettings *)
{}

void FormManagerPreferencesWidget::changeEvent(QEvent *e)
{}
