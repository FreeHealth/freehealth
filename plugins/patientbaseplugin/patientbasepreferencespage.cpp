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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "patientbasepreferencespage.h"
#include "constants_settings.h"
#include "constants_db.h"
#include "patientselector.h"

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <QUuid>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QProgressDialog>
#include <QDir>
#include <QByteArray>
#include <QBuffer>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;


static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

PatientBasePreferencesPage::PatientBasePreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{ setObjectName("PatientBasePreferencesPage"); }

PatientBasePreferencesPage::~PatientBasePreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString PatientBasePreferencesPage::id() const { return objectName(); }
QString PatientBasePreferencesPage::name() const { return tkTr(Trans::Constants::PATIENTS); }
QString PatientBasePreferencesPage::category() const { return tkTr(Trans::Constants::PATIENT_DATABASE); }

void PatientBasePreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
}

void PatientBasePreferencesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void PatientBasePreferencesPage::finish() { delete m_Widget; }

void PatientBasePreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Constants::S_SELECTOR_FIELDSTOSHOW, PatientSelector::Default);
    defaultvalues.insert(Constants::S_SELECTOR_SHOWVIRTUALPATIENTS, true);
    defaultvalues.insert(Constants::S_SELECTOR_USEGENDERCOLORS, true);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *PatientBasePreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new PatientBasePreferencesWidget(parent);
    return m_Widget;
}



PatientBasePreferencesWidget::PatientBasePreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    setDatasToUi();
}

void PatientBasePreferencesWidget::setDatasToUi()
{
    showVirtualPatients->setChecked(settings()->value(Constants::S_SELECTOR_SHOWVIRTUALPATIENTS).toBool());
    genderColor->setChecked(settings()->value(Constants::S_SELECTOR_USEGENDERCOLORS).toBool());
}

void PatientBasePreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    s->setValue(Constants::S_SELECTOR_SHOWVIRTUALPATIENTS, showVirtualPatients->isChecked());
    s->setValue(Constants::S_SELECTOR_USEGENDERCOLORS, genderColor->isChecked());
}

void PatientBasePreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    //    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("PatientBasePreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("FreeDiamsMainWindow"));
    s->setValue(Constants::S_SELECTOR_SHOWVIRTUALPATIENTS, true);
    s->setValue(Constants::S_SELECTOR_USEGENDERCOLORS, true);
    s->sync();
}

void PatientBasePreferencesWidget::changeEvent(QEvent *e)
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
