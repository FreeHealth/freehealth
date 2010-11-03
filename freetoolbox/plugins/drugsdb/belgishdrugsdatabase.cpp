/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "belgishdrugsdatabase.h"
#include "ui_belgishdrugsdatabase.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <QDir>


// Download at : http://www.fagg-afmps.be/fr/items-HOME/Bases_de_donnees/index.jsp
// Convert with : http://www.mdb2mysql.com/

using namespace DrugsDbCreator;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

//static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/CanadianRawSources/");}
//static inline QString databaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/drugs/drugs-en_CA.db");}

//static inline QString databasePreparationScript()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SQL_IN_PATH).toString() + "/canadian_db_preparation.sql");}
//static inline QString databaseFinalizationScript() {return QDir::cleanPath(settings()->value(Core::Constants::S_SQL_IN_PATH).toString() + "/canadian_db_finalize.sql");}

static inline QString drugsDatabaseSqlSchema() {return settings()->value(Core::Constants::S_SQL_IN_PATH).toString() + QString(Core::Constants::FILE_DRUGS_DATABASE_SCHEME);}


BelgishDrugsDatabase::BelgishDrugsDatabase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BelgishDrugsDatabase)
{
    ui->setupUi(this);
}

BelgishDrugsDatabase::~BelgishDrugsDatabase()
{
    delete ui;
}

void BelgishDrugsDatabase::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
