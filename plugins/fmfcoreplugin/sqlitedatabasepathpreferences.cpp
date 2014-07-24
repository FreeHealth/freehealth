/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "sqlitedatabasepathpreferences.h"
#include "ui_sqlitedatabasepathpreferences.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>

using namespace Core;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

SqliteDatabasePathPage::SqliteDatabasePathPage(QObject *parent) :
    IOptionsPage(parent),
    m_Widget(0)
{
    setObjectName("SqliteDatabasePathPage");
}

SqliteDatabasePathPage::~SqliteDatabasePathPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

QString SqliteDatabasePathPage::id() const
{
    return objectName();
}

QString SqliteDatabasePathPage::displayName() const
{
    return tr("Database path");
}

QString SqliteDatabasePathPage::category() const
{
    return tkTr(Trans::Constants::GENERAL_PREFERENCES).remove("&");
}

QString SqliteDatabasePathPage::title() const
{
    return tr("Database path");
}

int SqliteDatabasePathPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_MAIN + 100;
}

void SqliteDatabasePathPage::finish()
{
    delete m_Widget;
}

QWidget *SqliteDatabasePathPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new SqliteDatabasePathWidget(parent);
    return m_Widget;
}

SqliteDatabasePathWidget::SqliteDatabasePathWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SqliteDatabasePathWidget)
{
    ui->setupUi(this);
    ui->path->setPromptDialogTitle(tr("Select a path"));
    ui->path->setExpectedKind(Utils::PathChooser::Directory);
    ui->path->setPath(settings()->databaseConnector().absPathToSqliteReadWriteDatabase());
    connect(ui->moveDatabase, SIGNAL(pressed()), this, SLOT(onMoveDatabaseRequested()));
}

SqliteDatabasePathWidget::~SqliteDatabasePathWidget()
{
    delete ui;
}

void SqliteDatabasePathWidget::saveToSettings(Core::ISettings *)
{
}

void SqliteDatabasePathWidget::onMoveDatabaseRequested()
{
    if (!ui->path->validatePath(ui->path->path()))
        return;

    // Get current path
    Utils::DatabaseConnector c = settings()->databaseConnector();
    QString source = c.absPathToSqliteReadWriteDatabase();
    QString dest = ui->path->path();
    // Copy all database to the selected path
    Utils::copyDir(source, dest);
    // Remove old databases
    Utils::removeDirRecursively(source);
    // Set path
    c.setAbsPathToReadWriteSqliteDatabase(dest);
    settings()->setDatabaseConnector(c);
    Utils::warningMessageBox(tr("Database moved"),
                             tr("All your database are now moved to:\n"
                                "%1\n\n"
                                "<b>You must restart the application.</b>")
                             .arg(dest));
}

//void SqliteDatabasePathWidget::changeEvent(QEvent *e)
//{
//}

