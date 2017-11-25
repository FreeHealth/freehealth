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
#include "saverestorepage.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

#include <quazip/quazip/quazipfile.h>

#include <QFileDialog>
#include <QProgressDialog>


/**
  \todo Manage MySQL databases.
*/

using namespace SaveRestore;
using namespace SaveRestore::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

namespace SaveRestore {
    namespace Constants {
        const char * const COMMENT_TAG = "FREEMEDFORMS ARCHIVE";
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  DrugsUserPage  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
SaveRestorePage::SaveRestorePage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("SaveRestorePage"); }

SaveRestorePage::~SaveRestorePage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString SaveRestorePage::id() const { return objectName(); }
QString SaveRestorePage::displayName() const { return tkTr(Trans::Constants::USER_DATA); }
QString SaveRestorePage::category() const { return tkTr(Trans::Constants::SAVE_AND_RESTORE); }
QString SaveRestorePage::title() const {return tr("Save / Restore / Import / Export user's datas");}
int SaveRestorePage::sortIndex() const {return 0;}

void SaveRestorePage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void SaveRestorePage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void SaveRestorePage::finish() { delete m_Widget; }

void SaveRestorePage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *SaveRestorePage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new SaveRestoreWidget(parent);
    return m_Widget;
}





// Path selector
// Sauvegarder
// Restaurer
// Exporter XML
// Importer depuis XML ou SQLite




SaveRestoreWidget::SaveRestoreWidget(QWidget *parent) :
        QWidget(parent)
{
    setObjectName("SaveRestoreWidget");
    setupUi(this);
    setDatasToUi();
}

void SaveRestoreWidget::setDatasToUi()
{
}

void SaveRestoreWidget::on_saveButton_clicked()
{
    QString z;
    QString f = QFileDialog::getSaveFileName(this,
                                             tkTr(Trans::Constants::FILESAVEAS_TEXT),
                                             QDir::homePath(),
                                             tkTr(Trans::Constants::FILE_FILTER_ZIP), &z,
                                             QFileDialog::ReadOnly);
    if (f.isEmpty())
        return;

    if (!f.endsWith(".zip"))
        f += ".zip";

    Utils::Log::addMessage(this, "Saving settings and datas.");

    char c;
    bool error = false;
    int i = 0;
    QDir userPath = QDir(settings()->path(Core::ISettings::UserResourcesPath));
    QFileInfoList files = Utils::getFiles(userPath);
    QFile inFile;
    QuaZip zip(f);
    QuaZipFile outFile(&zip);
    QProgressDialog dlg(tr("Saving user's datas"), tr("Cancel"), 0, files.count(), this);
    dlg.setWindowModality(Qt::WindowModal);

    if (!zip.open(QuaZip::mdCreate)) {
        Utils::Log::addError(this, QString("QuaZip error while creating zip file : %1").arg(outFile.getZipError()));
        return;
    }

    foreach(QFileInfo file, files) {
        dlg.setValue(i);
        ++i;
        int size = file.size() / 1024;
        if (size > 1024) {
            size = size / 1024;
            dlg.setLabelText(tr("Saving user's datas : %1 (%2 Mo)").arg(file.fileName()).arg(size));
        } else {
            dlg.setLabelText(tr("Saving user's datas : %1 (%2 Ko)").arg(file.fileName()).arg(size));
        }

        if (!file.isFile())
            continue;

        qApp->processEvents();
        if (dlg.wasCanceled())
            break;

        inFile.setFileName(file.absoluteFilePath());

        if (!inFile.open(QIODevice::ReadOnly)) {
            Utils::Log::addError(this, tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(file.fileName()));
            error = true;
            continue;
        }

        QuaZipNewInfo info(userPath.relativeFilePath(file.absoluteFilePath()), inFile.fileName());
        info.comment = Constants::COMMENT_TAG;

        if (!outFile.open(QIODevice::WriteOnly, info)) {
            Utils::Log::addError(this, QString("QuaZip error : %1").arg(outFile.getZipError()));
            error = true;
            continue;
        }

        while ((inFile.getChar(&c)) && (outFile.putChar(c))) ;
        if (outFile.getZipError() != UNZ_OK) {
            Utils::Log::addError(this, QString("QuaZip error while setting content : %1").arg(outFile.getZipError()));
            error = true;
            continue;
        }

        outFile.close();
        if(outFile.getZipError() != UNZ_OK) {
            Utils::Log::addError(this, QString("QuaZip error while closing file : %1").arg(outFile.getZipError()));
            error = true;
            continue;
        }
        inFile.close();
    }
    dlg.setValue(dlg.maximum());

    zip.close();

    if (zip.getZipError() != 0) {
        Utils::Log::addError(this, QString("QuaZip error while closing zip file : %1").arg(outFile.getZipError()));
        return;
    }

    if (!error) {
        Utils::informativeMessageBox(tr("User's datas correctly saved into %1").arg(zip.getZipName()),
                                     tr("This archive can be used to restore your datas and settings."));
    } else {
        Utils::informativeMessageBox(tr("Error when trying to save user's datas into %1").arg(zip.getZipName()),
                                     tr("This archive can not be used to restore your datas and settings."));
    }
}

bool SaveRestoreWidget::on_restoreButton_clicked()
{
    bool yes = Utils::yesNoMessageBox(tr("Restore settings and datas from an archive. Application restart needed."),
                           tr("The application must be restarted after the restoring process. All actual edition will be lost.\n"
                              "Do you really want to restore your settings and datas ?"));

    if (!yes)
        return true;

    QString f = QFileDialog::getOpenFileName(this, tr("Select an archive"),
                                             QDir::homePath(),
                                             tkTr(Trans::Constants::FILE_FILTER_ZIP));
    if (f.isEmpty())
        return true;

    settings()->sync();

    Utils::Log::addMessage(this, "Restoring settings and datas.");

    //    QString f = "/Users/eric/Desktop/essai/essai.zip";
    QuaZip zip(f);

    if (!zip.open(QuaZip::mdUnzip)) {
        Utils::Log::addError(this, tr("Unable to open the Zip archive %1. Error : %2.").arg(f).arg(zip.getZipError()));
        return false;
    }

    zip.setFileNameCodec("UTF-8");

    QuaZipFileInfo info;
    QuaZipFile file(&zip);
    QFile out;
    QString name;
    char c;
    bool error = false;
    for (bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {

        if (!zip.getCurrentFileInfo(&info)) {
            Utils::Log::addError(this, tr("Unable to get information about archived file. Error %1").arg(zip.getZipError()));
            return false;
        }

        if ((info.comment != Constants::COMMENT_TAG)) {
            Utils::Log::addError(this, tr("This file (%1) is not a valid archive.").arg(info.name));
            continue;
        }

        if (!file.open(QIODevice::ReadOnly)) {
            Utils::Log::addError(this, tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(file.getFileName()));
            error = true;
            continue;
        }

        name = file.getActualFileName();

        if (file.getZipError() != UNZ_OK) {
            qWarning("testRead(): file.getFileName(): %d", file.getZipError());
            error = true;
            continue;
        }

        //        out.setFileName("/Users/eric/Desktop/essai/out/"+name);
        out.setFileName(settings()->path(Core::ISettings::UserResourcesPath) + QDir::separator() + name);

        QString outFilePath = QFileInfo(out.fileName()).path();
        QDir outDir(outFilePath);
        if (!outDir.exists()) {
            if (!QDir().mkpath(outFilePath))
                Utils::Log::addError(this, tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg(outFilePath));
        }

        if (!out.open(QIODevice::WriteOnly)) {
            Utils::Log::addError(this, tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(out.fileName()));
            continue;
        }
        // Slow like hell (on GNU/Linux at least), but it is not my fault.
        // Not ZIP/UNZIP package's fault either.
        // The slowest thing here is out.putChar(c).
        while (file.getChar(&c)) out.putChar(c);
        out.close();

        if (file.getZipError() != UNZ_OK) {
            Utils::Log::addError(this, QString("QuaZip error while closing file : %1").arg(file.getZipError()));
            error = true;
            continue;
        }
        if (!file.atEnd()) {
            Utils::Log::addError(this, QString("QuaZip error while closing file : %1").arg(file.getZipError()));
            error = true;
            continue;
        }
        file.close();
        if (file.getZipError() != UNZ_OK) {
            Utils::Log::addError(this, QString("QuaZip error while closing file : %1").arg(file.getZipError()));
            error = true;
            continue;
        }
    }
    zip.close();
    if (zip.getZipError() != UNZ_OK) {
        Utils::Log::addError(this, QString("QuaZip error while closing file : %1").arg(zip.getZipError()));
        error = true;
    }

    // Close and restart application
    if (!error) {
        Utils::informativeMessageBox(tr("Your settings and datas are restored. Restart the application."),
                                     tr("Your datas have been restored. The application will close know."));
    } else {
        Utils::informativeMessageBox(tr("Your settings and datas can not be restored. Restart the application."),
                                     tr("Your datas can not be restored. The application will close know."));
    }
    Utils::Log::addMessage(this, "Settings and datas restored");

    qApp->exit();
    return true;
}

void SaveRestoreWidget::on_importButton_clicked()
{
}

void SaveRestoreWidget::on_exportButton_clicked()
{
}

void SaveRestoreWidget::saveToSettings(Core::ISettings *)
{
}

void SaveRestoreWidget::writeDefaultSettings(Core::ISettings *)
{
}

void SaveRestoreWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
