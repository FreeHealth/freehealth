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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pathpreferences.h"
#include "ui_pathpreferences.h"

#include <coreplugin/ftb_constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QDir>
#include <QDesktopServices>

using namespace Core;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }


/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  PathPreferencesPage  /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
PathPreferencesPage::PathPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("PathPreferencesPage"); }

PathPreferencesPage::~PathPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString PathPreferencesPage::id() const { return objectName(); }
QString PathPreferencesPage::displayName() const { return tkTr(Trans::Constants::PATH); }
QString PathPreferencesPage::category() const { return tkTr(Trans::Constants::GENERAL); }

void PathPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void PathPreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void PathPreferencesPage::checkSettingsValidity()
{
    QString docPath = settings()->path(Core::ISettings::UserDocumentsPath);

    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Constants::S_FILEOUTPUT_PATH, QString("%1/Files/").arg(docPath));
    defaultvalues.insert(Constants::S_DBOUTPUT_PATH, QString("%1/Databases/").arg(docPath));
    defaultvalues.insert(Constants::S_DATAPACK_SERVER_OUTPUT_PATH, QString("%1/DataPacks/").arg(docPath));
    defaultvalues.insert(Constants::S_TMP_PATH, QString("%1/Tmp/").arg(docPath));
    defaultvalues.insert(Constants::S_GITFILES_PATH, QString());

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k).isNull())
            settings()->setValue(k, defaultvalues.value(k));
    }

    // Check dirs / create them if needed
    QStringList keys;
    keys << Constants::S_FILEOUTPUT_PATH << Constants::S_DBOUTPUT_PATH << Constants::S_TMP_PATH;
    foreach(const QString &key, keys) {
        if (!Utils::checkDir(settings()->value(key).toString(), true, this->objectName()))
            Utils::warningMessageBox(tr("Can not use and/or create the following directory\n%1").arg(settings()->value(key).toString()),
                                     tr("Please go in the application preferences and select a valid directory."));
    }

    // Create Preferred paths
    foreach(const QString &key, defaultvalues.keys()) {
        QString path = settings()->value(key).toString();
        if (!QDir(path).exists()) {
            if (!QDir().mkpath(path))
                LOG_ERROR(tkTr(Trans::Constants::PATH_1_CANNOT_BE_CREATED).arg(path));
        }
    }

    settings()->sync();
}

void PathPreferencesPage::finish() { delete m_Widget; }

QWidget *PathPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new PathPreferencesWidget(parent);
    return m_Widget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  PathPreferencesWidgetPrivate  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
PathPreferencesWidget::PathPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::PathPreferencesWidget)
{
    ui->setupUi(this);
    setDataToUi();
}

PathPreferencesWidget::~PathPreferencesWidget()
{
    delete ui;
    ui = 0;
}

void PathPreferencesWidget::setDataToUi()
{
    QString s;
    s = settings()->value(Constants::S_DBOUTPUT_PATH).toString();
    ui->dblabel->setText(tr("Select the databases output path"));
    ui->dbpath->setPromptDialogTitle(tr("Select the databases output path"));
    ui->dbpath->setInitialBrowsePathBackup(s);
    ui->dbpath->setPath(s);

    s = settings()->value(Constants::S_FILEOUTPUT_PATH).toString();
    ui->filelabel->setText(tr("Select the files output path"));
    ui->filepath->setPromptDialogTitle(tr("Select the files output path"));
    ui->filepath->setInitialBrowsePathBackup(s);
    ui->filepath->setPath(s);

    s = settings()->value(Constants::S_DATAPACK_SERVER_OUTPUT_PATH).toString();
    ui->datapacklabel->setText(tr("Select the datapack server output path"));
    ui->datapackpath->setPromptDialogTitle(tr("Select the datapack server output path"));
    ui->datapackpath->setInitialBrowsePathBackup(s);
    ui->datapackpath->setPath(s);

    s = settings()->value(Constants::S_TMP_PATH).toString();
    ui->tmplabel->setText(tr("Select the temporary path"));
    ui->tmppath->setPromptDialogTitle(tr("Select the temporary path"));
    ui->tmppath->setInitialBrowsePathBackup(s);
    ui->tmppath->setPath(s);

    s = settings()->value(Constants::S_GITFILES_PATH).toString();
    ui->svnlabel->setText(tr("Select the GIT checkout path"));
    ui->svnpath->setPromptDialogTitle(tr("Select the GIT checkout path"));
    ui->svnpath->setInitialBrowsePathBackup(s);
    ui->svnpath->setPath(s);
}


void PathPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Core::ISettings *set = s;
    if (!set) {
        set = settings();
    }
    Utils::Log::addMessage("PathPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("PathPreferencesWidget"));

    QString docPath = settings()->path(Core::ISettings::UserDocumentsPath);

    set->setValue(Constants::S_FILEOUTPUT_PATH, QString("%1/Files/").arg(docPath));
    set->setValue(Constants::S_DBOUTPUT_PATH, QString("%1/Databases/").arg(docPath));
    set->setValue(Constants::S_DATAPACK_SERVER_OUTPUT_PATH, QString("%1/DataPacks/").arg(docPath));
    set->setValue(Constants::S_TMP_PATH, QString("%1/Tmp/").arg(docPath));
    set->setValue(Constants::S_GITFILES_PATH, QString());
    set->sync();
}

void PathPreferencesWidget::saveToSettings(Core::ISettings *s)
{
    Core::ISettings *set = s;
    if (!set) {
        set = settings();
    }

    set->setValue(Constants::S_FILEOUTPUT_PATH, ui->filepath->path());
    set->setValue(Constants::S_DBOUTPUT_PATH, ui->dbpath->path());
    set->setValue(Constants::S_TMP_PATH, ui->tmppath->path());
    set->setValue(Constants::S_DATAPACK_SERVER_OUTPUT_PATH, ui->datapackpath->path());
    set->setValue(Constants::S_GITFILES_PATH, ui->svnpath->path());

    set->sync();
}

void PathPreferencesWidget::changeEvent(QEvent *e)
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
