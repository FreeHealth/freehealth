/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
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
#include <ddiplugin/database/ddidatabasereporter.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/idocumentprinter.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <extensionsystem/pluginmanager.h>

#include "ui_databasereportdialog.h"

#include <QClipboard>
#include <QPushButton>

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}

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
    QPushButton *clip = d->ui->buttonBox->addButton(tr("To clipboard"), QDialogButtonBox::ActionRole);
    QPushButton *print = d->ui->buttonBox->addButton(tr("Print"), QDialogButtonBox::ActionRole);
    connect(clip, SIGNAL(clicked()), this, SLOT(onClipboardCopyRequested()));
    connect(print, SIGNAL(clicked()), this, SLOT(onPrintRequested()));

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
    Internal::DDIDatabaseReporter reporter;
    reporter.initialize();

    QString report = "<pre>";
    report += reporter.plainTextFullReport();
    report += "</pre>";

    d->ui->textBrowser->setHtml(report);

    return true;
}

void DatabaseReportDialog::onClipboardCopyRequested()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(d->ui->textBrowser->toPlainText());
}

void DatabaseReportDialog::onPrintRequested()
{
    Core::IDocumentPrinter *p = printer();
    if (!p) {
        LOG_ERROR("No IDocumentPrinter found");
        return;
    }

    p->clearTokens();
    QHash<QString, QVariant> tokens;
    tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, this->windowTitle());
    p->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);

    QString html;
    html  = "<html><body style=\"font-size: small\"><pre>";
    html += d->ui->textBrowser->toPlainText();
    html += "</pre></body></html>";

    p->print(html);
}
