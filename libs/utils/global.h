/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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

enum IOMode {
    Overwrite = 0,
    AppendToFile
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
UTILS_EXPORT void setFullScreen( QWidget *win, bool on );
UTILS_EXPORT void resizeAndCenter(QWidget *widget, QWidget *reference = 0);

// Message Boxes and default dialogs
UTILS_EXPORT void informativeMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
UTILS_EXPORT void warningMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
UTILS_EXPORT bool yesNoMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null, const QPixmap &icon = QPixmap() );
UTILS_EXPORT int withButtonsMessageBox( const QString &text, const QString&infoText, const QString&detail, const QStringList &buttonsText, const QString &title = QString::null, bool withCancelButton = false );
UTILS_EXPORT int withButtonsMessageBox( const QString &text, const QString&infoText, const QString&detail, QMessageBox::StandardButtons buts, QMessageBox::StandardButton defaultButton, const QString &title = QString::null );
UTILS_EXPORT bool okCancelMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
UTILS_EXPORT bool functionNotAvailableMessageBox( const QString &functionText );
UTILS_EXPORT void quickDebugDialog( const QStringList &texts);
UTILS_EXPORT bool defaultLicenceAgreementDialog(const QString &message, Utils::LicenseTerms::AvailableLicense licence);

// Dialogs for user input
UTILS_EXPORT QString askUser( const QString &title, const QString &question );

// Working with files and dirs
UTILS_EXPORT bool removeDir(const QString &name, QString *error);
UTILS_EXPORT QFileInfoList getFiles(QDir fromDir, const QStringList &filters, bool recursive = true);
UTILS_EXPORT QFileInfoList getFiles(QDir fromDir, const QString &filters = QString::null, bool recursive = true);
UTILS_EXPORT QFileInfoList getDirs(QDir fromDir, const QStringList &filters, bool recursive = true);
UTILS_EXPORT bool checkDir( const QString &absPath, bool createIfNotExist, const QString & logDirName = QString::null );
UTILS_EXPORT bool saveStringToFile( const QString &toSave, const QString &toFile, IOMode mode = Overwrite, const Warn warnUser = WarnUser, QWidget *parent=0 );
UTILS_EXPORT bool saveStringToFile( const QString &toSave,  const QString &dirPath, const QString &filters, QWidget *parent = 0 );
UTILS_EXPORT QString readTextFile( const QString &toRead, const QString &encoder, const Warn warnUser = WarnUser, QWidget *parent = 0 );
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
