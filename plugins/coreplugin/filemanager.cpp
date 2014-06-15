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
 ***************************************************************************/
/**
  \class Core::FileManager
  Manages recent file context.\n
  Settings management :
  - You can save/get from settings using getRecentFilesFromSettings() and saveRecentFiles()
  - You can set a specific settings key (just after instanciation) using setSettingsKey()
  - The Core::ICore::instance()->settings() is used when accessing to settings
  Recent managers are limited to 10 entries.
*/
#include "filemanager.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <QDir>
#include <QDebug>

using namespace Core;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

/** CTor*/
FileManager::FileManager(QObject *parent)
    : QObject(parent),
      m_maxRecentFiles(10)
{
    setObjectName("FileManager");
}

/*!
    Adds the \a fileName to the list of recent files.
*/
void FileManager::addToRecentFiles(const QString &fileName)
{
    if (fileName.isEmpty())
        return;
    QString prettyFileName(QDir::toNativeSeparators(fileName));
    m_recentFiles.removeAll(prettyFileName);
    if (m_recentFiles.count() > m_maxRecentFiles)
        m_recentFiles.removeLast();
    m_recentFiles.prepend(prettyFileName);
}

/*!
    Returns the list of recent files.
*/
QStringList FileManager::recentFiles() const
{
    return m_recentFiles;
}

/** Save the recorded recent files and the maximum number of records in the Core::ISettings */
void FileManager::saveRecentFiles() const
{
    Core::ISettings *s = settings();
    s->beginGroup(Constants::S_RECENTFILES_GROUP);
    if (m_Key.isEmpty()) {
        s->setValue(Constants::S_RECENTFILES_KEY, m_recentFiles);
        s->setValue(Constants::S_RECENTFILES_MAX_KEY, m_maxRecentFiles);
    } else {
        s->setValue(m_Key, m_recentFiles);
        s->setValue(m_Key + "/" + Constants::S_RECENTFILES_MAX_KEY, m_maxRecentFiles);
    }
    s->endGroup();
}

/** Get the recent recorded from the global Core::ISettings */
void FileManager::getRecentFilesFromSettings()
{
    Core::ISettings *s = settings();
    m_recentFiles.clear();
    s->beginGroup(Constants::S_RECENTFILES_GROUP);
    if (m_Key.isEmpty())
        m_recentFiles = s->value(Constants::S_RECENTFILES_KEY).toStringList();
    else
        m_recentFiles = s->value(m_Key).toStringList();
    s->endGroup();
}

/** Get the maximum of recent records from the global Core::ISettings */
void FileManager::getMaximumRecentFilesFromSettings()
{
    Core::ISettings *s = settings();
    s->beginGroup(Constants::S_RECENTFILES_GROUP);
    if (m_Key.isEmpty())
        m_maxRecentFiles = s->value(Constants::S_RECENTFILES_MAX_KEY, m_maxRecentFiles).toInt();
    else
        m_maxRecentFiles = s->value(m_Key + "/" + Constants::S_RECENTFILES_MAX_KEY, m_maxRecentFiles).toInt();
    s->endGroup();
}

/*!
  The current file is e.g. the file currently opened when an editor is active,
  or the selected file in case a Project Explorer is active ...
  \sa currentFile
  */
void FileManager::setCurrentFile(const QString &filePath)
{
    if (m_currentFile == filePath)
        return;
    m_currentFile = filePath;
//    emit currentFileChanged(m_currentFile);
}

/*!
  Returns the absolute path of the current file
  The current file is e.g. the file currently opened when an editor is active,
  or the selected file in case a Project Explorer is active ...
  \sa setCurrentFile
  */
QString FileManager::currentFile() const
{
    return m_currentFile;
}
