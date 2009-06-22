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
#ifndef TKGLOBAL_H
#define TKGLOBAL_H

#include <tkExporter.h>

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
 * \file tkGlobal.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.14
 * \date 18 Jun 2009
*/

Q_TK_BEGIN_CONSTANTS
const char* const TOKEN_OPEN   = "[";
const char* const TOKEN_CLOSE  = "]";
Q_TK_END_CONSTANTS

Q_TK_BEGIN_GLOBAL

enum Warn {
    WarnUser = 0,
    DontWarnUser
};


/**  \brief Initialize the lib. Should be be called before all. Initialize : tkTranslators, tkTheme. */
Q_TK_EXPORT const void initLib();

// Libs and OS informations
/**  \brief Returns informations about the librairy (version, name, date of compilation). */
Q_TK_EXPORT const QString getLibraryInformations();
/** \brief Return true if the application was build in debug mode. */
Q_TK_EXPORT bool isDebugCompilation();
/** \brief Return true if the application was build using the SVN. */
Q_TK_EXPORT bool isSvnBuild();
/** \brief Return true if the application was build with all private parts. */
Q_TK_EXPORT bool isFullApplication();
/** \brief Return true if the application is running on MacOs. */
Q_TK_EXPORT bool isRunningOnMac();
/** \brief Return true if the application is running on Windows. */
Q_TK_EXPORT bool isRunningOnWin();
/** \brief Return true if the application is running on Linux. */
Q_TK_EXPORT bool isRunningOnLinux();
/** \brief Return true if the application is running on FreeBsd. */
Q_TK_EXPORT bool isRunningOnFreebsd();
/** \brief Returns informations of 'uname -a' command. Works on Linux, MacOs, FreeBSD */
Q_TK_EXPORT QString uname();
Q_TK_EXPORT QString osName();

// Widget geometry
/** \brief Center a widget into the screen. */
Q_TK_EXPORT const void centerWidget( QWidget * win );
/** \brief Toggle a widget to fullScreen. */
Q_TK_EXPORT const void setFullScreen( QWidget* win, bool on );

// Message Boxes
Q_TK_EXPORT const void informativeMessageBox( const QString &text, const QString&infoText, const QString&detail, const QString &title = QString::null );
Q_TK_EXPORT const void warningMessageBox( const QString &text, const QString&infoText, const QString&detail, const QString &title = QString::null );
Q_TK_EXPORT const bool yesNoMessageBox( const QString &text, const QString&infoText, const QString&detail, const QString &title = QString::null );
Q_TK_EXPORT const int withButtonsMessageBox( const QString &text, const QString&infoText, const QString&detail, const QStringList &buttonsText, const QString &title = QString::null );
Q_TK_EXPORT const int withButtonsMessageBox( const QString &text, const QString&infoText, const QString&detail, QMessageBox::StandardButtons buts, QMessageBox::StandardButton defaultButton, const QString &title = QString::null );
Q_TK_EXPORT const bool okCancelMessageBox( const QString &text, const QString&infoText, const QString&detail, const QString &title = QString::null );
Q_TK_EXPORT const bool functionNotAvailableMessageBox( const QString &functionText );
Q_TK_EXPORT const void quickDebugDialog( const QStringList &texts);

// Dialogs for user input
Q_TK_EXPORT const QString askUser( const QString &title, const QString &question );

// Working with files and dirs
Q_TK_EXPORT const QFileInfoList getFiles( QDir fromDir, const QStringList& filters, bool recursive = true );
Q_TK_EXPORT const QFileInfoList getFiles( QDir fromDir, const QString& filters = QString::null, bool recursive = true );
Q_TK_EXPORT const bool checkDir( const QString & absPath, bool createIfNotExist, const QString & logDirName = QString::null );
Q_TK_EXPORT const bool saveStringToFile( const QString &toSave, const QString &toFile, const Warn warnUser = WarnUser, QWidget *parent=0 );
Q_TK_EXPORT const bool saveStringToFile( const QString &toSave,  const QString &dirPath, const QString &filters, QWidget *parent = 0 );
Q_TK_EXPORT const QString readTextFile( const QString &toRead, const Warn warnUser = WarnUser, QWidget *parent = 0 );
Q_TK_EXPORT const QString isDirExists(const QString &absPath);
Q_TK_EXPORT const QString isFileExists(const QString &absPath);

// Some usefull widgets specific functions
/** \brief Toggle the echomode of a QLineEdit. */
Q_TK_EXPORT const void switchEchoMode( QLineEdit * l );

// Some functions for HTML work
Q_TK_EXPORT const QString fontToHtml( const QFont &font, const QString &color = QString::null );
Q_TK_EXPORT const QString textAlignmentToHtml( const Qt::Alignment &align );
Q_TK_EXPORT const QString toHtmlAccent( const QString &html );

// Some functions for XML Handling
Q_TK_EXPORT const QString createXml( const QString &mainTag, const QHash<QString,QString> &datas, const int indent = 0, const bool valueToBase64 = false );
Q_TK_EXPORT const bool    readXml( const QString &xmlContent, const QString &generalTag, QHash<QString,QString> &readDatas,const bool valueFromBase64 = false );
Q_TK_EXPORT const QString xmlRead(const QDomElement &father, const QString &name, const QString &defaultValue);
Q_TK_EXPORT const QString xmlRead(const QDomElement &father, const QString &name, const char *defaultValue);
Q_TK_EXPORT const int     xmlRead(const QDomElement &father, const QString &name, const int defaultValue);
Q_TK_EXPORT const int     xmlRead(const QDomElement &father, const QString &name, const long int defaultValue);
Q_TK_EXPORT const bool    xmlRead(const QDomElement &father, const QString &name, const bool defaultValue);
Q_TK_EXPORT const void    xmlWrite(QDomElement &father, const QString &name, const QString &value);
Q_TK_EXPORT const void    xmlWrite(QDomElement &father, const QString &name, char *value);
Q_TK_EXPORT const void    xmlWrite(QDomElement &father, const QString &name, int value);
Q_TK_EXPORT const void    xmlWrite(QDomElement &father, const QString &name, long int value);
Q_TK_EXPORT const void    xmlWrite(QDomElement &father, const QString &name, bool value);

// Some functions for token replacement [ text added if token exists [TOKEN] text added if token exists ]
Q_TK_EXPORT const int replaceToken( QString &textToAnalyse, const QString &token, const QString &value );

Q_TK_END_GLOBAL

#endif  // TKGLOBAL_H
