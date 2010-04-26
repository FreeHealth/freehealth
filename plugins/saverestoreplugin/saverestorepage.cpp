/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
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
QString SaveRestorePage::name() const { return tkTr(Trans::Constants::USER_DATAS); }
QString SaveRestorePage::category() const { return tkTr(Trans::Constants::SAVE_AND_RESTORE); }

void SaveRestorePage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void SaveRestorePage::applyChanges()
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
    QDir userPath = QDir(settings()->path(Core::ISettings::ResourcesPath));
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

        while (inFile.getChar(&c) && outFile.putChar(c));
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
            Utils::Log::addError(this, tr("Unable to get informations about archived file. Error %1").arg(zip.getZipError()));
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
        out.setFileName(settings()->path(Core::ISettings::ResourcesPath) + QDir::separator() + name);

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
