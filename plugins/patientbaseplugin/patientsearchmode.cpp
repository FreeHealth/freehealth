/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2016 by Eric MAEKER, MD                                      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Patients::Internal::PatientSearchMode
 * \internal
 * \brief Core::IMode for the Patient selector.
 * \sa Core::IMode
*/

#include "patientsearchmode.h"
#include "patientselector.h"
#include "patientcore.h"
#include "patientwidgetmanager.h"
#include "constants_menus.h"

#include <coreplugin/icore.h>
#include <coreplugin/theme.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include <translationutils/constanttranslations.h>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

PatientSearchMode::PatientSearchMode(QObject *parent) :
    Core::IMode(parent),
    m_Selector(0)
{
    setDisplayName(tkTr(Trans::Constants::PATIENTS));
    setIcon(theme()->icon(Core::Constants::ICONPATIENTS, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_PATIENT_SEARCH);
    setId(Core::Constants::MODE_PATIENT_SEARCH);
    Core::Context context(Constants::C_PATIENTS, Constants::C_PATIENTS_SEARCH);
    setContext(context);
    setPatientBarVisibility(false);

    // Add the new patient action in the mode manager
    Core::Command *cmd = actionManager()->command(Core::Constants::A_PATIENT_NEW);
    modeManager()->addAction(cmd->action(), Core::Constants::P_MODE_PATIENT_SEARCH);

    // create the mode widget
    m_Selector = new PatientSelector;
    PatientCore::instance()->patientWidgetManager()->setCurrentView(m_Selector);
    m_Selector->setFieldsToShow(PatientSelector::Title | PatientSelector::UsualName | PatientSelector::OtherNames | PatientSelector::FirstName | PatientSelector::DateOfBirth | PatientSelector::FullAddress);
    m_Selector->initialize();
    setWidget(m_Selector);
}

PatientSearchMode::~PatientSearchMode()
{
    if (m_Selector)
        delete m_Selector;
}

void PatientSearchMode::postCoreInitialization()
{
}
