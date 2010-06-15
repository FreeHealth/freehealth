/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
 ***************************************************************************/
#ifndef CORE_FILEMANAGER_H
#define CORE_FILEMANAGER_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/idebugpage.h>

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
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.2
 * \date 13 Sept 2009
*/

namespace Core {

class CORE_EXPORT FileManager : public QObject
{
    Q_OBJECT

public:
    FileManager(QObject *parent=0) : QObject(parent) {setObjectName("FileManager");}
    virtual ~FileManager() {}

    // recent files
    void getRecentFilesFromSettings();
    void addToRecentFiles(const QString &fileName);
    void setRecentFiles(const QStringList &files) {m_recentFiles = files;}
    QStringList recentFiles() const;
    void saveRecentFiles() const;

    // current file
    void setCurrentFile(const QString &filePath);
    QString currentFile() const;

    // settings
    void setSettingsKey(const QString &key) {m_Key = key;}
    QString settingsKey() const {return m_Key;}

private:
    QStringList m_recentFiles;
    static const int m_maxRecentFiles = 10;
    QString m_currentFile;
    QString m_Key;
};

}  // end Core

#endif  // CORE_FILEMANAGER_H
