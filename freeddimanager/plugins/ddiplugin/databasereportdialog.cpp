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
 * \class DDI::DatabaseReportDialog
 */

#include "databasereportdialog.h"
#include <ddiplugin/ddicore.h>
#include <ddiplugin/database/ddidatabase.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>

#include "ui_databasereportdialog.h"

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DDI::DDICore *ddiCore() {return DDI::DDICore::instance();}
static inline DDI::Internal::DDIDatabase &ddiBase() {return ddiCore()->database();}

namespace DDI {
namespace Internal {
class DatabaseReportDialogPrivate
{
public:
    DatabaseReportDialogPrivate(DatabaseReportDialog *parent) :
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~DatabaseReportDialogPrivate()
    {
        delete ui;
    }
    
public:
    Ui::DatabaseReportDialog *ui;
    
private:
    DatabaseReportDialog *q;
};
} // namespace Internal
} // end namespace DDI


/*! Constructor of the DDI::DatabaseReportDialog class */
DatabaseReportDialog::DatabaseReportDialog(QWidget *parent) :
    QDialog(parent),
    d(new DatabaseReportDialogPrivate(this))
{
    d->ui = new Ui::DatabaseReportDialog;
    d->ui->setupUi(this);

    setWindowTitle(tkTr(Trans::Constants::DATABASE_INFORMATION));
    setWindowIcon(theme()->icon(Core::Constants::ICONINFORMATION));
    Utils::resizeAndCenter(this, parent);
}

/*! Destructor of the DDI::DatabaseReportDialog class */
DatabaseReportDialog::~DatabaseReportDialog()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the dialog with the database report and returns \e true. */
bool DatabaseReportDialog::initialize()
{
    // Get all reports from the database
    QString report = "<pre>";
    report += ddiBase().plainTextAtcReport();
    report += "\n\n";
    report += ddiBase().plainTextInteractorsReport();
    report += "\n\n";
    report += ddiBase().plainTextDrugDrugInteractionsReport();
    report += "</pre>";

    d->ui->textBrowser->setHtml(report);

    return true;
}

