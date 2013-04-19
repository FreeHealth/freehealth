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
 * \class Patients::Internal::FormExporterDialog
 * \brief short description of class
 *
 * Long description of class
 * \sa Form::
 */

#include "formexporterdialog.h"

#include <formmanagerplugin/formexporter.h>

#include <translationutils/constants.h>

#include "ui_formexporterdialog.h"

#include <QDebug>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Patients {
namespace Internal {
class FormExporterDialogPrivate
{
public:
    FormExporterDialogPrivate(FormExporterDialog *parent) :
        _initialized(false),
        _exporter(0),
        ui(new Ui::FormExporterDialog),
        q(parent)
    {
    }
    
    ~FormExporterDialogPrivate()
    {
        delete ui;
    }
    
public:
    bool _initialized;
    Form::FormExporter *_exporter;
    Ui::FormExporterDialog *ui;
    
private:
    FormExporterDialog *q;
};
} // namespace Internal
} // end namespace Patients


/*! Constructor of the Patients::Internal::FormExporterDialog class */
FormExporterDialog::FormExporterDialog(QWidget *parent) :
    QDialog(parent),
    d(new FormExporterDialogPrivate(this))
{
    d->_exporter = new Form::FormExporter(this);
}

/*! Destructor of the Patients::Internal::FormExporterDialog class */
FormExporterDialog::~FormExporterDialog()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool FormExporterDialog::initialize()
{
    if (d->_initialized)
        return true;
    return true;
}

