/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
#ifndef UTILSGLOBAL_H
#define UTILSGLOBAL_H

#include <utils/global_exporter.h>
#include <utils/licenseterms.h>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QDir;
class QFont;
class QDomElement;
class QWidget;
QT_END_NAMESPACE

#include <QFileInfo>
#include <QString>
#include <QHash>
#include <QMessageBox>

/**
 * \file global.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.18
 * \date 10 Aug 2009
*/

namespace Utils {

namespace Constants {
const char* const TOKEN_OPEN   = "[";
const char* const TOKEN_CLOSE  = "]";
}

enum Warn {
    WarnUser = 0,
    DontWarnUser
};


// Libs and OS informations
/** \brief Return true if the application was build in debug mode. */
UTILS_EXPORT bool isDebugCompilation();
/** \brief Return true if the application was build using the SVN. */
UTILS_EXPORT bool isSvnBuild();
/** \brief Return true if the application was build with all private parts. */
UTILS_EXPORT bool isFullApplication();
/** \brief Return true if the application is running on MacOs. */
UTILS_EXPORT bool isRunningOnMac();
/** \brief Return true if the application is running on Windows. */
UTILS_EXPORT bool isRunningOnWin();
/** \brief Return true if the application is running on Linux. */
UTILS_EXPORT bool isRunningOnLinux();
/** \brief Return true if the application is running on FreeBsd. */
UTILS_EXPORT bool isRunningOnFreebsd();
/** \brief Returns informations of 'uname -a' command. Works on Linux, MacOs, FreeBSD */
UTILS_EXPORT QString uname();
UTILS_EXPORT QString osName();

// Widget geometry
/** \brief Center a widget \e winToCenter into the \e reference widget. If no \e reference is passed, the \e winToCenter is centered into the desktop. */
UTILS_EXPORT void centerWidget(QWidget *winToCenter, QWidget *reference = 0);
/** \brief Toggle a widget to fullScreen. */
UTILS_EXPORT void setFullScreen( QWidget* win, bool on );

// Message Boxes and default dialogs
UTILS_EXPORT void informativeMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
UTILS_EXPORT void warningMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
UTILS_EXPORT bool yesNoMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
UTILS_EXPORT int withButtonsMessageBox( const QString &text, const QString&infoText, const QString&detail, const QStringList &buttonsText, const QString &title = QString::null );
UTILS_EXPORT int withButtonsMessageBox( const QString &text, const QString&infoText, const QString&detail, QMessageBox::StandardButtons buts, QMessageBox::StandardButton defaultButton, const QString &title = QString::null );
UTILS_EXPORT bool okCancelMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
UTILS_EXPORT bool functionNotAvailableMessageBox( const QString &functionText );
UTILS_EXPORT void quickDebugDialog( const QStringList &texts);
UTILS_EXPORT bool defaultLicenceAgreementDialog(const QString &message, Utils::LicenseTerms::AvailableLicense licence);

// Dialogs for user input
UTILS_EXPORT QString askUser( const QString &title, const QString &question );

// Working with files and dirs
UTILS_EXPORT bool removeDir(const QString &name, QString *error);
UTILS_EXPORT QFileInfoList getFiles( QDir fromDir, const QStringList& filters, bool recursive = true );
UTILS_EXPORT QFileInfoList getFiles( QDir fromDir, const QString& filters = QString::null, bool recursive = true );
UTILS_EXPORT bool checkDir( const QString & absPath, bool createIfNotExist, const QString & logDirName = QString::null );
UTILS_EXPORT bool saveStringToFile( const QString &toSave, const QString &toFile, const Warn warnUser = WarnUser, QWidget *parent=0 );
UTILS_EXPORT bool saveStringToFile( const QString &toSave,  const QString &dirPath, const QString &filters, QWidget *parent = 0 );
UTILS_EXPORT QString readTextFile( const QString &toRead, const Warn warnUser = WarnUser, QWidget *parent = 0 );
UTILS_EXPORT QString isDirExists(const QString &absPath);
UTILS_EXPORT QString isFileExists(const QString &absPath);

// Some usefull widgets specific functions
/** \brief Toggle the echomode of a QLineEdit. */
UTILS_EXPORT void switchEchoMode( QLineEdit * l );

// Some functions for HTML work
UTILS_EXPORT QString fontToHtml( const QFont &font, const QColor &color = QColor("black") );
UTILS_EXPORT QString textAlignmentToHtml( const Qt::Alignment &align );
UTILS_EXPORT QString toHtmlAccent( const QString &html );

// Some functions for XML Handling
UTILS_EXPORT QString createXml( const QString &mainTag, const QHash<QString,QString> &datas, const int indent = 0, const bool valueToBase64 = false );
UTILS_EXPORT bool    readXml( const QString &xmlContent, const QString &generalTag, QHash<QString,QString> &readDatas,const bool valueFromBase64 = false );
UTILS_EXPORT QString xmlRead(const QDomElement &father, const QString &name, const QString &defaultValue);
UTILS_EXPORT QString xmlRead(const QDomElement &father, const QString &name, const char *defaultValue);
UTILS_EXPORT int     xmlRead(const QDomElement &father, const QString &name, const int defaultValue);
UTILS_EXPORT int     xmlRead(const QDomElement &father, const QString &name, const long int defaultValue);
UTILS_EXPORT bool    xmlRead(const QDomElement &father, const QString &name, const bool defaultValue);
UTILS_EXPORT void    xmlWrite(QDomElement &father, const QString &name, const QString &value);
UTILS_EXPORT void    xmlWrite(QDomElement &father, const QString &name, char *value);
UTILS_EXPORT void    xmlWrite(QDomElement &father, const QString &name, int value);
UTILS_EXPORT void    xmlWrite(QDomElement &father, const QString &name, long int value);
UTILS_EXPORT void    xmlWrite(QDomElement &father, const QString &name, bool value);

// Some functions for token replacement [ text added if token exists [TOKEN] text added if token exists ]
UTILS_EXPORT int replaceToken( QString &textToAnalyse, const QString &token, const QString &value );
UTILS_EXPORT int replaceTokens(QString &textToAnalyse, const QHash<QString, QString> &tokens_values);

}  // end Utils

#endif  // UTILSGLOBAL_H
