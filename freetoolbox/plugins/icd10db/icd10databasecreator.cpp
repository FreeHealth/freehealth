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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@free.fr>               *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "icd10databasecreator.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <icdplugin/icddownloader.h>

#include <utils/log.h>

#include <QFile>
#include <QDir>

#include <QDebug>

#include "ui_icd10databasewidget.h"

using namespace Icd10;

const char* const  ICD10_URL               = "http://www.fda.gov/downloads/Drugs/InformationOnDrugs/ucm054599.zip";
const char* const  ICD10_DATABASE_NAME     =  "FDA_US";

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/Icd10RawSources/") + QDir::separator();}
static inline QString databaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/icd10/icd10.db");}

static inline QString databaseCreationScript()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SQL_IN_PATH).toString() + "/icd10.sql");}


Icd10DatabasePage::Icd10DatabasePage(QObject *parent) :
        IToolPage(parent)
{
    setObjectName("Icd10DatabasePage_IToolPage");
}

QWidget *Icd10DatabasePage::createPage(QWidget *parent)
{
    return new Icd10DatabaseWidget(parent);
}


Icd10DatabaseWidget::Icd10DatabaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Icd10DatabaseWidget)
{
    setObjectName("Icd10DatabaseWidget");
    ui->setupUi(this);
    m_WorkingPath = workingPath();
    if (!QDir().mkpath(m_WorkingPath))
        Utils::Log::addError(this, "Unable to create ICD10 Working Path :" + m_WorkingPath, __FILE__, __LINE__);
    else
        Utils::Log::addMessage(this, "Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath))
            Utils::Log::addError(this, "Unable to create ICD10 database output path :" + dbpath, __FILE__, __LINE__);
        else
            Utils::Log::addMessage(this, "ICD10 database output dir created");
    }
}

Icd10DatabaseWidget::~Icd10DatabaseWidget()
{
    delete ui;
}

bool Icd10DatabaseWidget::on_startCreation_clicked()
{
    ICD::IcdDownloader *dld = new ICD::IcdDownloader(this);
    dld->setMainWindow(mainwindow());
    dld->setOutputPath(m_WorkingPath);
    dld->setUrl(QUrl(ICD10_URL));
    dld->startDownload();
    connect(dld, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
    connect(dld, SIGNAL(downloadFinished()), dld, SLOT(deleteLater()));
    return true;
}


void Icd10DatabaseWidget::changeEvent(QEvent *e)
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

