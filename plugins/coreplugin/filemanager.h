/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef CORE_FILEMANAGER_H
#define CORE_FILEMANAGER_H

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QWidget>
#include <QVariant>
#include <QPointer>
#include <QHash>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE


/**
 * \file filemanager.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 12 Apr 2013
*/

namespace Core {

class CORE_EXPORT FileManager : public QObject
{
    Q_OBJECT

public:
    FileManager(QObject *parent = 0);
    virtual ~FileManager() {}

    // settings management
    void setSettingsKey(const QString &key) {m_Key = key;}
    QString settingsKey() const {return m_Key;}
    void saveRecentFiles() const;
    void getRecentFilesFromSettings();
    void getMaximumRecentFilesFromSettings();

    // Recent files management
    void addToRecentFiles(const QString &fileName);
    void setRecentFiles(const QStringList &files) {m_recentFiles = files;}
    QStringList recentFiles() const;
    void setMaximumRecentFiles(int max) {m_maxRecentFiles = max;}

    // current file
    void setCurrentFile(const QString &filePath);
    QString currentFile() const;

private:
    QStringList m_recentFiles;
    int m_maxRecentFiles;
    QString m_currentFile;
    QString m_Key;
};

}  // end Core

#endif  // CORE_FILEMANAGER_H
