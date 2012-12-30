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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Tools::Internal::PdfTkWrapper
 */

#include "pdftkwrapper.h"

#include <translationutils/constants.h>

#include <QDebug>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Tools {
namespace Internal {
class PdfTkWrapperPrivate
{
public:
    PdfTkWrapperPrivate(PdfTkWrapper *parent) :
        _initialized(false),
        q(parent)
    {
    }
    
    ~PdfTkWrapperPrivate()
    {
    }
    
public:
    // Put your data here
    bool _initialized;

private:
    PdfTkWrapper *q;
};
} // namespace Internal
} // end namespace Tools


/*! Constructor of the Tools::Internal::PdfTkWrapper class */
PdfTkWrapper::PdfTkWrapper(QObject *parent) :
    QObject(parent),
    d(new PdfTkWrapperPrivate(this))
{
}

/*! Destructor of the Tools::Internal::PdfTkWrapper class */
PdfTkWrapper::~PdfTkWrapper()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool PdfTkWrapper::initialize()
{
    if (d->_initialized)
        return true;
    // Check if a pdftk binary exists
    // TODO: Security assessment: Check bin MD5
    return true;
}

/** Check that everything is Ok with this binary wrapper */
bool PdfTkWrapper::isAvailable() const
{
    return d->_initialized;
}

QString PdfTkWrapper::fillPdfWithFdf(const QString &absFileName, const QString &fdfContent)
{

}
