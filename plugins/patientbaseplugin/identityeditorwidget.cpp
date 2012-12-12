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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class Patients::IdentityEditorWidget
 * \brief Identity editor.
 * This widget allow user to view & edit the identity of a patient.
 * You can define the Patients::PatientModel and the index to use,
 * or just keep the identity always sync with the Core::IPatient current
 * patient.
*/

#include "identityeditorwidget.h"
#include "patientmodel.h"
#include "patientbase.h"
#include "patientcore.h"
#include "constants_db.h"

#include "ui_identitywidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/iphotoprovider.h>
#include <patientbaseplugin/constants_settings.h>

#include <zipcodesplugin/zipcodescompleters.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/uppercasevalidator.h>
#include <extensionsystem/pluginmanager.h>
#include <translationutils/constants.h>

#include <QDir>
#include <QFileDialog>
#include <QDateEdit>

#include <QDebug>
#include <QMenu>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Patients::Internal::PatientBase *patientBase() {return Patients::Internal::PatientBase::instance();}
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Patients::PatientCore *patientCore() {return Patients::PatientCore::instance();}

//TODO: Users can add pages in the identity widget using the XMLForm --> create a <Form> named \e Identity

namespace Patients {
namespace Internal {
class IdentityEditorWidgetPrivate
{
public:
    IdentityEditorWidgetPrivate(IdentityEditorWidget *parent) :
        m_PatientModel(0),
        q(parent)
    {
    }

    ~IdentityEditorWidgetPrivate()
    {
    }

public:
    Patients::PatientModel *m_PatientModel;

private:
    IdentityEditorWidget *q;
};
}  // namespace Internal
}  // namespace Patients

IdentityEditorWidget::IdentityEditorWidget(QWidget *parent) :
    Identity::IdentityEditorWidget(parent),
    d(new Internal::IdentityEditorWidgetPrivate(this))
{
    setObjectName("Patient::IdentityEditorWidget");
    initialize();
}

IdentityEditorWidget::~IdentityEditorWidget()
{
    if (d)
        delete d;
    d = 0;
}

/** \brief Test the validity of the "actually shown" identity. */
bool IdentityEditorWidget::isIdentityValid() const
{
    // check database for double entries
    QString where = QString("`%1`='%2' AND ").
            arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_BIRTHNAME),
                currentBirthName());
    if (!currentSecondName().isEmpty())
        where += QString("`%1`='%2' AND ").
                arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_SECONDNAME),
                    currentSecondName());
    where += QString("`%1`='%2'").
            arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_FIRSTNAME),
                currentFirstName());
    return (patientBase()->count(Constants::Table_IDENT, Constants::IDENTITY_BIRTHNAME, where)>0);
}

