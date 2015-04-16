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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class PMH::Internal::PmhTokens
 */

#include "pmhtokens.h"
#include "pmhcategorymodel.h"

#include <coreplugin/constants_tokensandsettings.h>

#include <translationutils/constants.h>

#include <QDebug>

using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace PMH {
namespace Internal {
/*!
 * \class PMH::Internal::PmhTokensPrivate
 * \brief Private implementation of the PMH::PmhTokens class.
 *
 * documentation here
 */
class PmhTokensPrivate
{
public:
    PmhTokensPrivate(PmhTokens *parent) :
        _type(PmhTokens::HtmlOutput),
        _model(0),
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~PmhTokensPrivate()
    {
    }
    
public:
    PmhTokens::OutputType _type;
    QString _uid;
    PmhCategoryModel *_model;

private:
    PmhTokens *q;
};
} // namespace Internal
} // end namespace PMH


/*! Constructor of the PMH::Internal::PmhTokens class */
PmhTokens::PmhTokens() :
    Core::IToken(QString::null),
    d(new PmhTokensPrivate(this))
{
}

/*! Destructor of the PMH::Internal::PmhTokens class */
PmhTokens::~PmhTokens()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool PmhTokens::initialize(PmhCategoryModel *model)
{
    d->_model = model;
    return true;
}

void PmhTokens::setOutputType(OutputType type)
{
    d->_type = type;
    QString uid;
    switch (type) {
    case HtmlOutput:
        uid = QString("%1.%2.%3")
               .arg(Core::Constants::TOKEN_PATIENT_NAMESPACE)
               .arg("Pmhx")
               .arg("Html");
        break;
    case PlainTextOutput:
        uid = QString("%1.%2.%3")
               .arg(Core::Constants::TOKEN_PATIENT_NAMESPACE)
               .arg("Pmhx")
               .arg("PlainText");
        break;
    }
    setUid(uid);
}

QString PmhTokens::tooltip() const
{
    return "Patient PMHx overview";
}

QVariant PmhTokens::testValue() const
{
    return QString("Patient PMHx overview: " + uid());
}

QVariant PmhTokens::value() const
{
    // TODO: Tokens: code this
    switch (d->_type) {
    case HtmlOutput:
        break;
    case PlainTextOutput:
        break;
    }
    return d->_model->synthesis();
}
