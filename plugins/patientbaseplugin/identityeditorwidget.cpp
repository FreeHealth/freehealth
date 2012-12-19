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
 * \brief Wrapper over the Identity::IdentityEditorWidget
 * Check the identity validity.
*/

#include "identityeditorwidget.h"
#include "patientbase.h"
#include "constants_db.h"

#include <QDebug>

using namespace Patients;
using namespace Internal;

static inline Patients::Internal::PatientBase *patientBase() {return Patients::Internal::PatientBase::instance();}

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

