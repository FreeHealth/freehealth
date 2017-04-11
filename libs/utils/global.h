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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_GLOBAL_H
#define UTILS_GLOBAL_H

#include <utils/global_exporter.h>
#include <utils/licenseterms.h>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QDir;
class QFont;
class QDomElement;
class QWidget;
class QModelIndex;
class QAbstractItemModel;
QT_END_NAMESPACE

#include <QFileInfo>
#include <QString>
#include <QHash>
#include <QMessageBox>
#include <QLocale>

/**
 * \file global.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.4
 * \date 19 June 2012
*/

#define IN_RANGE(v, min, max)            (min <= v && v <= max)
#define IN_RANGE_STRICTLY(v, min, max)   (min < v && v < max)
#define IN_RANGE_STRICT_MAX(v, min, max) (min <= v && v < max)

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

enum DirSearchType {
    Recursively = 0,
    NotRecursively
};

// Libs and OS information
/** \brief Return true if the application was built in debug mode. */
UTILS_EXPORT bool isDebugWithoutInstallCompilation();
/** \brief Return true if the application was built in release mode. */
UTILS_EXPORT bool isReleaseCompilation();
/** \brief Return true if the application was built using a GIT checkout. */
UTILS_EXPORT bool isGitBuild();
/** \brief Return true if the application was built with all private parts. */
UTILS_EXPORT bool isFullApplication();
/** \brief Return true if the application is an alpha build (release or debug). */
UTILS_EXPORT bool isAlpha();
/** \brief Return true if the application is a beta. */
UTILS_EXPORT bool isBeta();
/** \brief Return true if the application is a release candidate. */
UTILS_EXPORT bool isReleaseCandidate();
/** \brief Return true if the application is running on MacOs. */
UTILS_EXPORT bool isRunningOnMac();
/** \brief Return true if the application is running on Windows. */
UTILS_EXPORT bool isRunningOnWin();
/** \brief Return true if the application is running on Linux. */
UTILS_EXPORT bool isRunningOnLinux();
/** \brief Return true if the application was built was the LINUX_INTEGRATED tag. */
UTILS_EXPORT bool isLinuxIntegratedCompilation();
/** \brief Returns information of 'uname -a' command. Works on Linux, MacOs. */
UTILS_EXPORT QString uname();
UTILS_EXPORT QString osName();
UTILS_EXPORT QStringList applicationPluginsPath(const QString &binaryName, const QString &libraryBaseName);

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
UTILS_EXPORT bool defaultLicenseAgreementDialog(const QString &message, Utils::LicenseTerms::AvailableLicense license);

// Dialogs for user input
UTILS_EXPORT QString askUser( const QString &title, const QString &question );

// Working with files and dirs
UTILS_EXPORT bool copyDir(const QString &absSourcePath, const QString &absDestPath);
UTILS_EXPORT bool removeDir(const QString &absPath, QString *error);
UTILS_EXPORT bool removeDirRecursively(const QString &absPath, QString *error = 0);
UTILS_EXPORT QFileInfoList getFiles(QDir fromDir, const QStringList &filters, DirSearchType recursive = Recursively);
UTILS_EXPORT QFileInfoList getFiles(QDir fromDir, const QString &filters = QString::null, DirSearchType recursive = Recursively);
UTILS_EXPORT QFileInfoList getDirs(QDir fromDir, const QStringList &filters, DirSearchType recursive = Recursively);
UTILS_EXPORT bool checkDir( const QString &absPath, bool createIfNotExist, const QString & logDirName = QString::null);
UTILS_EXPORT bool saveStringToFile( const QString &toSave, const QString &toFile, IOMode mode = Overwrite, const Warn warnUser = WarnUser, QWidget *parent = 0);
UTILS_EXPORT bool saveStringToEncodedFile( const QString &toSave, const QString &toFile, const QString &forceEncoding, IOMode mode = Overwrite, const Warn warnUser = WarnUser, QWidget *parent = 0);
UTILS_EXPORT bool saveStringToFile( const QString &toSave,  const QString &dirPath, const QString &filters, const QString &message = QString::null, QWidget *parent = 0);
UTILS_EXPORT QString readTextFile( const QString &toRead, const Warn warnUser = WarnUser);
UTILS_EXPORT QString isDirExists(const QString &absPath);
UTILS_EXPORT QString isFileExists(const QString &absPath);
UTILS_EXPORT QByteArray fileMd5(const QString &fileName);
UTILS_EXPORT QByteArray fileSha1(const QString &fileName);
UTILS_EXPORT QByteArray fileSha256(const QString &fileName);
UTILS_EXPORT QString humanReadableFileSize(qint64 size);

// Some useful widgets specific functions
UTILS_EXPORT void switchEchoMode( QLineEdit * l );

// Some date & numbering functions
UTILS_EXPORT QDateTime roundDateTime(const QDateTime &date, const int minutesRound);
UTILS_EXPORT bool inRange(const int min, const int max, const int value);
UTILS_EXPORT bool inRange(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QModelIndex &value);

// Some locale functions
UTILS_EXPORT QString countryToIso(QLocale::Country country);
UTILS_EXPORT QString countryIsoToName(const QString &country);
UTILS_EXPORT QLocale::Country countryIsoToCountry(const QString &country);

// Some functions for QString/HTML work
UTILS_EXPORT QString htmlBodyContent(const QString &fullHtml, bool replaceBodyTagByParagraphTag = true);
UTILS_EXPORT QString htmlTakeAllCssContent(QString &fullHtml);
UTILS_EXPORT QString htmlReplaceParagraphWithDiv(const QString &fullHtml);
UTILS_EXPORT QString htmlRemoveLinkTags(const QString &fullHtml);
UTILS_EXPORT QString fontToHtml( const QFont &font, const QColor &color = QColor("black") );
UTILS_EXPORT QString textAlignmentToHtml( const Qt::Alignment &align );
UTILS_EXPORT QString htmlReplaceAccents(const QString &html);
UTILS_EXPORT QStringList htmlGetLinksToCssContent(const QString &html);
UTILS_EXPORT QString firstLetterUpperCase(const QString &s);
UTILS_EXPORT QString correctTextAccentEncoding(const QString &text);
UTILS_EXPORT QString removeAccents(const QString &text);
UTILS_EXPORT QString lineWrapString(const QString &in, int lineLength);
UTILS_EXPORT QString centerString(const QString &in, const QChar &fill, int lineSize);
UTILS_EXPORT QString indentString(const QString &in, int lineIndent);

// Some functions for XML Handling
UTILS_EXPORT QString createXml( const QString &mainTag, const QHash<QString,QString> &data, const int indent = 0, const bool valueToBase64 = false );
UTILS_EXPORT bool    readXml( const QString &xmlContent, const QString &generalTag, QHash<QString,QString> &readData, const bool valueFromBase64 = false );
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

// Some pixmap transformations
UTILS_EXPORT QByteArray pixmapToBase64(const QPixmap &pixmap);
UTILS_EXPORT QPixmap pixmapFromBase64(const QByteArray &base64);

// Some functions for token replacement [ text added if token exists [TOKEN] text added if token exists ]
UTILS_EXPORT int replaceToken( QString &textToAnalyse, const QString &token, const QString &value );
UTILS_EXPORT int replaceTokens(QString &textToAnalyse, const QHash<QString, QString> &tokens_values);
UTILS_EXPORT int replaceTokens(QString &textToAnalyse, const QHash<QString, QVariant> &tokens_values);

// Some network helpers
UTILS_EXPORT QString testInternetConnection();

// Some database identifiers helpers
UTILS_EXPORT QString createUid();

// Models
UTILS_EXPORT void linkSignalsFromFirstModelToSecondModel(QAbstractItemModel *model1, QAbstractItemModel *model2, bool connectDataChanged);

// List/Vector
UTILS_EXPORT QVector<int> removeDuplicates(const QVector<int> &vector);
UTILS_EXPORT QList<int> removeDuplicates(const QList<int> &list);

}  // end Utils

#endif  // UTILS_GLOBAL_H
