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
#include "global.h"

#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_msgerror.h>
#include <utils/log.h>

#include <QApplication>
#include <QWidget>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QLineEdit>
#include <QTextCodec>
#include <QByteArray>
#include <QFont>
#include <QFileDialog>
#include <QDomNode>
#include <QDomDocument>
#include <QInputDialog>
#include <QProcess>
#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QTextDocument>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QAbstractButton>
#include <QTextDocument>
#include <QCryptographicHash>
#include <QMainWindow>
#include <QModelIndex>
#include <QNetworkConfigurationManager>
#include <QBuffer>
#include <QUuid>

/**
 * \namespace Utils
 * \brief Some global funtions and classes for the applications.
 * \note Many Unit-tests are available (see: tests/auto/auto.pro)
 */

using namespace Utils;
using namespace Trans::ConstantTranslations;

namespace {
static const unsigned char two_letter_country_code_list[] =
"  " // Default
"AF" // Afghanistan
"AL" // Albania
"DZ" // Algeria
"AS" // AmericanSamoa
"AD" // Andorra
"AO" // Angola
"AI" // Anguilla
"AQ" // Antarctica
"AG" // AntiguaAndBarbuda
"AR" // Argentina
"AM" // Armenia
"AW" // Aruba
"AU" // Australia
"AT" // Austria
"AZ" // Azerbaijan
"BS" // Bahamas
"BH" // Bahrain
"BD" // Bangladesh
"BB" // Barbados
"BY" // Belarus
"BE" // Belgium
"BZ" // Belize
"BJ" // Benin
"BM" // Bermuda
"BT" // Bhutan
"BO" // Bolivia
"BA" // BosniaAndHerzegowina
"BW" // Botswana
"BV" // BouvetIsland
"BR" // Brazil
"IO" // BritishIndianOceanTerritory
"BN" // BruneiDarussalam
"BG" // Bulgaria
"BF" // BurkinaFaso
"BI" // Burundi
"KH" // Cambodia
"CM" // Cameroon
"CA" // Canada
"CV" // CapeVerde
"KY" // CaymanIslands
"CF" // CentralAfricanRepublic
"TD" // Chad
"CL" // Chile
"CN" // China
"CX" // ChristmasIsland
"CC" // CocosIslands
"CO" // Colombia
"KM" // Comoros
"CD" // DemocraticRepublicOfCongo
"CG" // PeoplesRepublicOfCongo
"CK" // CookIslands
"CR" // CostaRica
"CI" // IvoryCoast
"HR" // Croatia
"CU" // Cuba
"CY" // Cyprus
"CZ" // CzechRepublic
"DK" // Denmark
"DJ" // Djibouti
"DM" // Dominica
"DO" // DominicanRepublic
"TL" // EastTimor
"EC" // Ecuador
"EG" // Egypt
"SV" // ElSalvador
"GQ" // EquatorialGuinea
"ER" // Eritrea
"EE" // Estonia
"ET" // Ethiopia
"FK" // FalklandIslands
"FO" // FaroeIslands
"FJ" // Fiji
"FI" // Finland
"FR" // France
"FX" // MetropolitanFrance
"GF" // FrenchGuiana
"PF" // FrenchPolynesia
"TF" // FrenchSouthernTerritories
"GA" // Gabon
"GM" // Gambia
"GE" // Georgia
"DE" // Germany
"GH" // Ghana
"GI" // Gibraltar
"GR" // Greece
"GL" // Greenland
"GD" // Grenada
"GP" // Guadeloupe
"GU" // Guam
"GT" // Guatemala
"GN" // Guinea
"GW" // GuineaBissau
"GY" // Guyana
"HT" // Haiti
"HM" // HeardAndMcDonaldIslands
"HN" // Honduras
"HK" // HongKong
"HU" // Hungary
"IS" // Iceland
"IN" // India
"ID" // Indonesia
"IR" // Iran
"IQ" // Iraq
"IE" // Ireland
"IL" // Israel
"IT" // Italy
"JM" // Jamaica
"JP" // Japan
"JO" // Jordan
"KZ" // Kazakhstan
"KE" // Kenya
"KI" // Kiribati
"KP" // DemocraticRepublicOfKorea
"KR" // RepublicOfKorea
"KW" // Kuwait
"KG" // Kyrgyzstan
"LA" // Lao
"LV" // Latvia
"LB" // Lebanon
"LS" // Lesotho
"LR" // Liberia
"LY" // LibyanArabJamahiriya
"LI" // Liechtenstein
"LT" // Lithuania
"LU" // Luxembourg
"MO" // Macau
"MK" // Macedonia
"MG" // Madagascar
"MW" // Malawi
"MY" // Malaysia
"MV" // Maldives
"ML" // Mali
"MT" // Malta
"MH" // MarshallIslands
"MQ" // Martinique
"MR" // Mauritania
"MU" // Mauritius
"YT" // Mayotte
"MX" // Mexico
"FM" // Micronesia
"MD" // Moldova
"MC" // Monaco
"MN" // Mongolia
"MS" // Montserrat
"MA" // Morocco
"MZ" // Mozambique
"MM" // Myanmar
"NA" // Namibia
"NR" // Nauru
"NP" // Nepal
"NL" // Netherlands
"AN" // NetherlandsAntilles
"NC" // NewCaledonia
"NZ" // NewZealand
"NI" // Nicaragua
"NE" // Niger
"NG" // Nigeria
"NU" // Niue
"NF" // NorfolkIsland
"MP" // NorthernMarianaIslands
"NO" // Norway
"OM" // Oman
"PK" // Pakistan
"PW" // Palau
"PS" // PalestinianTerritory
"PA" // Panama
"PG" // PapuaNewGuinea
"PY" // Paraguay
"PE" // Peru
"PH" // Philippines
"PN" // Pitcairn
"PL" // Poland
"PT" // Portugal
"PR" // PuertoRico
"QA" // Qatar
"RE" // Reunion
"RO" // Romania
"RU" // RussianFederation
"RW" // Rwanda
"KN" // SaintKittsAndNevis
"LC" // StLucia
"VC" // StVincentAndTheGrenadines
"WS" // Samoa
"SM" // SanMarino
"ST" // SaoTomeAndPrincipe
"SA" // SaudiArabia
"SN" // Senegal
"SC" // Seychelles
"SL" // SierraLeone
"SG" // Singapore
"SK" // Slovakia
"SI" // Slovenia
"SB" // SolomonIslands
"SO" // Somalia
"ZA" // SouthAfrica
"GS" // SouthGeorgiaAndTheSouthSandwichIslands
"ES" // Spain
"LK" // SriLanka
"SH" // StHelena
"PM" // StPierreAndMiquelon
"SD" // Sudan
"SR" // Suriname
"SJ" // SvalbardAndJanMayenIslands
"SZ" // Swaziland
"SE" // Sweden
"CH" // Switzerland
"SY" // SyrianArabRepublic
"TW" // Taiwan
"TJ" // Tajikistan
"TZ" // Tanzania
"TH" // Thailand
"TG" // Togo
"TK" // Tokelau
"TO" // Tonga
"TT" // TrinidadAndTobago
"TN" // Tunisia
"TR" // Turkey
"TM" // Turkmenistan
"TC" // TurksAndCaicosIslands
"TV" // Tuvalu
"UG" // Uganda
"UA" // Ukraine
"AE" // UnitedArabEmirates
"GB" // UnitedKingdom
"US" // UnitedStates
"UM" // UnitedStatesMinorOutlyingIslands
"UY" // Uruguay
"UZ" // Uzbekistan
"VU" // Vanuatu
"VA" // VaticanCityState
"VE" // Venezuela
"VN" // VietNam
"VG" // BritishVirginIslands
"VI" // USVirginIslands
"WF" // WallisAndFutunaIslands
"EH" // WesternSahara
"YE" // Yemen
"YU" // Yugoslavia
"ZM" // Zambia
"ZW" // Zimbabwe
"CS" // SerbiaAndMontenegro
;

} // End anonymous namespace


namespace Utils {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////   LIB AND OS FUNCTIONS   ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isDebugWithoutInstallCompilation()
{
#ifdef DEBUG_WITHOUT_INSTALL
    return true;
#else
    return false;
#endif
}

bool isReleaseCompilation()
{
#ifdef RELEASE
    return true;
#else
    return false;
#endif
}

bool isGitBuild()
{
#ifdef FULLAPPLICATION_BUILD
    return false;
#else
    return true;
#endif
}

bool isFullApplication()
{
#ifdef FULLAPPLICATION_BUILD
    return true;
#else
    return false;
#endif
}

bool isAlpha()
{
    return qApp->applicationVersion().contains("alpha", Qt::CaseInsensitive);
}

bool isBeta()
{
    return qApp->applicationVersion().contains("beta", Qt::CaseInsensitive);
}

bool isReleaseCandidate()
{
    return qApp->applicationVersion().contains("rc", Qt::CaseInsensitive);
}

bool isRunningOnMac()
{
#ifdef Q_OS_MAC
    return true;
#else
    return false;
#endif
}

bool isRunningOnWin()
{
#ifdef Q_OS_WIN
    return true;
#else
    return false;
#endif
}

bool isRunningOnLinux()
{
#ifdef Q_OS_LINUX
    return true;
#else
    return false;
#endif
}

bool isLinuxIntegratedCompilation()
{
#ifdef LINUX_INTEGRATED
    return true;
#else
    return false;
#endif
}

QString uname()
{
    QString system;
    if (isRunningOnMac())
        system = "MacOs";
    else if (isRunningOnLinux())
        system = "Linux";
    else if (isRunningOnWin())
        system = "Windows";
    if (system.isEmpty())
        return QString();
    system += ": ";
    QProcess uname;
    uname.start("uname", QStringList() << "-a");
    if (!uname.waitForStarted())
        LOG_ERROR_FOR("Utils", QApplication::translate("Utils", "Error while retrieve information of uname under %1").arg(system));
    if (!uname.waitForFinished())
        LOG_ERROR_FOR("Utils", QApplication::translate("Utils", "Error while retrieve information of uname under %1").arg(system));
    system += uname.readAll();
    return system.remove("\n");
}

QString osName()
{
    if (isRunningOnLinux())
        return "Linux";
    else if (isRunningOnMac())
        return "MacOs";
    else if (isRunningOnWin())
        return "Windows";
    return QString();
}

/** Returns the application plugin path. Also defines the QApplication::libraryPath(). */
QStringList applicationPluginsPath(const QString &binaryName, const QString &libraryBaseName)
{
    QString app = qApp->applicationDirPath();

    // Debug without installation compilation
    // Plugins are stored in the bin path next to the binaries (or binaries.app)
    // bin
    //  `- freemedhealth (dir)
    //       |- freehealth (exec)
    //       `- plugins (dir - contains all plugins and libs)
    if (isDebugWithoutInstallCompilation()) {
        if (isRunningOnMac()) {
            app.append("/../../../");
        }
        app += "/plugins/";
        qApp->addLibraryPath(QDir::cleanPath(app));
        return QStringList() << QDir::cleanPath(app);
    }

    // With the Linux integrated compilation, the 'make install' process is mandatory.
    // Bins are installed in : /usr/bin (or prefixed path)
    // Libs are installed in : /usr/lib(arch)/freehealth-common
    // Plugins are installed in : /usr/lib(arch)/applicationname
    if (isLinuxIntegratedCompilation()) {
        app = QString("/usr/%1/%2")
                .arg(libraryBaseName)
                .arg(QString(binaryName).remove("_debug").toLower());
        qApp->addLibraryPath(app);
        return QStringList() << app;
    }

    // 'make install' called on debug/release build
    if (isRunningOnMac()) {
        // Bundle structure
        // Application binary is in: application.app/Contents/MacOS
        // Libs & plugins are in: application.app/Contents/plugins
        app += "/../plugins/";
    } else if (isRunningOnWin()) {
        // In Windows bundle libs and plugins are stored in the 'plugins' path next to the binary
        app += "/plugins/";
    } else {
        // Idem by default
        app += "/plugins/";
    }

    // When the application is installed force Qt to search for libs/plugins/QtPlugins inside
    // the application bundle.
    QStringList lpath;
    lpath << QDir::cleanPath(app) << QDir::cleanPath(app + "/qt");
    qApp->setLibraryPaths(lpath);
    return lpath;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////   FILES FUNCTIONS   /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Copy all content of a path \e absSourcePath to a destination \e absDestPath. \n
 * The all contents are recursively included.\n
 * \warning When the destination already exists, source files are added without any cleaning
 * or checking. Be warned!
 */
bool copyDir(const QString &absSourcePath, const QString &absDestPath)
{
    // No source?
    if (!QDir(absSourcePath).exists())
        return false;

    // Create destination path
    if (!QDir(absDestPath).exists()) {
        if (!QDir().mkpath(absDestPath))
            return false;
    }

    // Get file content of the current directory
    QDir srcDir(absSourcePath);
    QFileInfoList files = Utils::getFiles(srcDir);
    // TODO: manage empty dirs
    foreach(const QFileInfo &info, files) {
        QString srcItem = info.absoluteFilePath();
        QString srcRel = QDir(absSourcePath).relativeFilePath(srcItem);
        if (srcRel.startsWith("..")) {
            LOG_ERROR_FOR("Tools", "Relative path outside source path tree");
            continue;
        }
        QString dstItem = QDir::cleanPath(QString("%1/%2")
                .arg(absDestPath)
                .arg(srcRel));
//        if (info.isDir()) {
//            if (!copyDir(srcItem, dstItem)) {
//                return false;
//            }
//        } else
        if (info.isFile()) {
            if (!QDir().mkpath(QFileInfo(dstItem).absolutePath()))
                return false;
            if (!QFile::copy(srcItem, dstItem)) {
                return false;
            }
        } else {
            qDebug() << "Unhandled item" << info.filePath() << "in Utils::copyDir()";
        }
    }
    return true;
}

/**
 * This function deletes all files in the selected dir (non recursively)
 * and tries to remove the dir
 * \sa Utils::removeDirRecursively()
 */
bool removeDir(const QString &name, QString *error)
{
    error->clear();
    QDir dir(name);
    if (!dir.exists()) {
        return true;
    }
    // is there is directory inside ? --> return false
    QStringList list = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (list.count()) {
        error->append(Trans::ConstantTranslations::tkTr(Trans::Constants::PATH_1_CONTAINS_DIRS).arg(name));
        return false;
    }
    // delete files
    list = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    foreach(const QString &f, list) {
        if (!dir.remove(f)) {
            error->append(Trans::ConstantTranslations::tkTr(Trans::Constants::FILE_1_CANNOT_BE_REMOVED).arg(f));
            return false;
        }
    }
    // delete dir
    if (!dir.rmpath(dir.absolutePath())) {
        error->append(Trans::ConstantTranslations::tkTr(Trans::Constants::PATH_1_CANNOT_BE_REMOVED).arg(dir.absolutePath()));
        return false;
    }
    return true;
}

/**
 * Remove dir recursively. All dir content will be deleted.
 * \sa Utils::removeDir()
 */
bool removeDirRecursively(const QString &absPath, QString *error)
{
    if (error)
        error->clear();
    QDir dir(absPath);
    if (!dir.exists())
        return true;

    // remove all dirs inside this dir (recursively)
    foreach(const QString &dirPath, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString er;
        if (!removeDirRecursively(dir.absolutePath() + QDir::separator() + dirPath, &er)) {
            if (error)
                error->append(er);
            return false;
        }
    }

    // remove this dir
    QString er;
    if (!removeDir(dir.absolutePath(), &er)) {
        if (error)
            error->append(er);
        return false;
    }

    return true;
}

/**
 * Search and return all files matching the \e filters from the path \e fromDir.
 * You can search recursively or not using the \e recursive param. \n
 * If \e fromDir path is "." this function returns an empty file list.
 */
QFileInfoList getFiles(QDir fromDir, const QStringList &filters, DirSearchType recursive)
{
    QFileInfoList files;
    // Don't proceed non-existing dirs
    if (!fromDir.exists())
        return files;
    if (fromDir.path() == ".")
        return files;
    // Get content
    foreach (const QFileInfo & file, fromDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name)) {
        if (file.isFile() && (filters.isEmpty() || QDir::match(filters, file.fileName())))
            files << file;
        else if (file.isDir() && recursive==Recursively) {
            fromDir.cd(file.filePath());
            files << getFiles(fromDir, filters, recursive);
            fromDir.cdUp();
        }
    }
    return files;
}

/**
 * Search and return all files matching the \e filter from the path \e fromDir.
 * You can search recursively or not using the \e recursive param.
 * \sa Utils::getDirs()
 */
QFileInfoList getFiles(QDir fromDir, const QString &filter, DirSearchType recursive)
{
    return getFiles(fromDir, filter.isEmpty() ? QStringList() : QStringList(filter), recursive);
}

/**
 * Search and return all directories matching the \e filters from the path \e fromDir.
 * You can search recursively or not using the \e recursive param.
 * \sa Utils::getFiles()
 */
QFileInfoList getDirs(QDir fromDir, const QStringList &filters, DirSearchType recursive)
{
    QFileInfoList dirs;
    foreach (const QFileInfo &file, fromDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::IgnoreCase)) {
        if (file.isFile() && (filters.isEmpty() || QDir::match(filters, file.fileName())))
            dirs << file;
        else if (file.isDir() && recursive==Recursively) {
            fromDir.cd(file.filePath());
            dirs << getFiles(fromDir, filters, recursive);
            fromDir.cdUp();
        }
    }
    return dirs;
}

/**
   \brief Test a dir, try to create it if it does not exist.
   \param absPath : AbsolutePath of the directory to test
   \param createIfNotExist : try to create the dir if true
   \param logDirName : logical name to use for warning (eg : "Application path") for the debug login
**/
bool checkDir(const QString &absPath, bool createIfNotExist, const QString &logDirName)
{
    if (!QFile::exists(absPath)) {
        if (createIfNotExist) {
            LOG_FOR("Utils", QCoreApplication::translate("Utils", "%1: %2 does not exist. Trying to create it.")
                               .arg(logDirName, absPath));
            if (!QDir().mkpath(absPath)) {
                LOG_ERROR_FOR("Utils", QCoreApplication::translate("Utils", "Unable to create the %1: %2.")
                              .arg(logDirName, absPath));
                return false;
            }
        } else {
            LOG_FOR("Utils", QCoreApplication::translate("Utils", "%1: %2 does not exist.")
                               .arg(logDirName, absPath));
            return false;
        }
    }
    return true;
}

/**
 * Save the string \e toSave to a text file \e toFile.
 * You can choose to warn the user or not if an error
 * is encountered. Return true if all went well.
 * By default, all files are saved in UTF8.
 */
bool saveStringToFile(const QString &toSave, const QString &toFile, IOMode iomode, const Warn warnUser, QWidget *parent)
{
    return saveStringToEncodedFile(toSave, toFile, "UTF-8", iomode, warnUser, parent);
}

/**
 * Save the string \e toSave to a text file \e toFile.
 * You can choose to warn the user or not is an error
 * is encountered. Return true if all gone good. \n
 * Set the file string encoding using the \e forceEncoding (see QTextCodec)
 */
bool saveStringToEncodedFile(const QString &toSave, const QString &toFile, const QString &forceEncoding, IOMode iomode, const Warn warnUser, QWidget *parent)
{
    if (toFile.isEmpty()) {
        LOG_ERROR_FOR("Utils", "saveStringToEncodedFile(): fileName is empty");
        return false;
    }
    QWidget *wgt = parent;
    if (!parent) {
        wgt = qApp->activeWindow();
    }

    // Manage relative paths
    QString correctFileName = toFile;
    QFileInfo info(toFile);
    if (info.isRelative())
        correctFileName = qApp->applicationDirPath() + QDir::separator() + toFile;
    info.setFile(correctFileName);

    // Save file (overwrite)
    QFile file(info.absoluteFilePath());
    if (info.exists() && (info.isWritable() && warnUser == WarnUser)) {
        if (QMessageBox::warning(wgt, qApp->applicationName(),
                                 QCoreApplication::translate("Utils" ,
                                                             "File %1 already exists. Do you want de replace it?").arg(info.fileName()),
                                 QMessageBox::Cancel | QMessageBox::Ok) == QMessageBox::Ok) {
            if (iomode == Overwrite) {
                if (!file.open(QFile::WriteOnly | QIODevice::Text)) {
                    LOG_ERROR_FOR("Utils", QCoreApplication::translate("Utils", "Error %1 while trying to save file %2").arg(file.fileName(), file.errorString()));
                    return false;
                }
            } else if (iomode == AppendToFile) {
                if (!file.open(QFile::Append | QIODevice::Text)) {
                    LOG_ERROR_FOR("Utils", QCoreApplication::translate("Utils", "Error %1 while trying to save file %2").arg(file.fileName(), file.errorString()));
                    return false;
                }
            } else {
                return false;
            }

            QTextCodec *codec = QTextCodec::codecForName(forceEncoding.toUtf8());
            if (!codec) {
                LOG_ERROR_FOR("Utils", "Codec not found: " + forceEncoding);
                // fallback to UTF8
                codec = QTextCodec::codecForName("UTF-8");
            }
            file.write(codec->fromUnicode(toSave));
            file.close();
            if (Log::debugFileInOutProcess())
                LOG_FOR("Utils", QCoreApplication::translate("Utils", "%1 successfully saved (%2)").arg(file.fileName()).arg(forceEncoding));
        } else {
            if (Log::debugFileInOutProcess())
                LOG_FOR("Utils", QCoreApplication::translate("Utils", "Save file aborted by user (file already exists): ") + file.fileName());
            return false;
        }
    } else {
        // Create file
        if (!file.open(QFile::WriteOnly | QIODevice::Text)) {
            LOG_ERROR_FOR("Utils", QCoreApplication::translate("Utils", "Error %1 while trying to save file %2").arg(file.fileName(),file.errorString()));
            return false;
        }

        QTextCodec *codec = QTextCodec::codecForName(forceEncoding.toUtf8());
        if (!codec) {
            LOG_ERROR_FOR("Utils", "Codec not found: " + forceEncoding);
            // fallback to UTF8
            codec = QTextCodec::codecForName("UTF-8");
        }
        file.write(codec->fromUnicode(toSave));
        file.close();
        if (Log::debugFileInOutProcess())
            LOG_FOR("Utils", QCoreApplication::translate("Utils", "%1 successfully saved (%2)").arg(file.fileName()).arg(forceEncoding));
    }
    return true;
}

/** \brief Save the string to a text file. Ask user for the name of the file to save. \sa  saveStringToFile() **/
bool saveStringToFile(const QString &toSave, const QString &dirPath, const QString &filters, const QString &message, QWidget *parent)
{
    QWidget *wgt = parent;
    if (!parent) {
        wgt = qApp->activeWindow();
    }
    QString fileName;
    if (message.isEmpty())
        fileName = QFileDialog::getSaveFileName(wgt,
                                                QCoreApplication::translate("Utils", "Save to file"),
                                                dirPath,
                                                filters);
    else
        fileName = QFileDialog::getSaveFileName(wgt,
                                                message,
                                                dirPath,
                                                filters);

    if (fileName.isEmpty())
        return false;
    return Utils::saveStringToFile(toSave, fileName, Overwrite, WarnUser, wgt);
}

/**
 \brief Return the content of a text file.
  You can choose:
  - to warn the user if an error is encountered.
  - the encoding of the file (using the QTextCodec supported encodings)
**/
QString readTextFile(const QString &toRead, const Warn warnUser)
{
    if (toRead.isEmpty())
        return QString();
    // Manage relative paths
    QString correctFileName = toRead;
    QFileInfo info(toRead);
    if (info.isRelative())
        correctFileName = qApp->applicationDirPath() + QDir::separator() + toRead;
    info.setFile(correctFileName);

    if (((!info.exists()) || (!info.isReadable())) && (warnUser == WarnUser)) {
        Utils::warningMessageBox(QCoreApplication::translate("Utils" , "File %1 does not exist or is not readable.").arg(correctFileName),
                                 "","", qApp->applicationName());
        return QString::null;
    } else {
        QFile file(correctFileName);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            LOG_ERROR_FOR("Utils", QCoreApplication::translate("Utils", "Error %1 while trying to open file %2")
                          .arg(correctFileName, file.errorString()));
            return QString::null;
        }
        QTextStream in(&file);
        in.setCodec("UTF-8");
        QString text;
        text = in.readAll();
        file.close();
        if (Log::debugFileInOutProcess())
            LOG_FOR("Utils", tkTr(Trans::Constants::FILE_1_LOADED).arg(toRead));
        return text;
    }
    return QString::null;
}

/** \brief Test a dir, if dir exists return the absDirName, otherwise return empty QString. **/
QString isDirExists(const QString &absPath)
{
    if (QDir(absPath).exists())
        return QDir::cleanPath(absPath);
    return QString();
}

/** \brief Test a file, if file exists return the absFileName, otherwise return empty QString. **/
QString isFileExists(const QString &absPath)
{
    if (QFile(absPath).exists())
        return QDir::cleanPath(absPath);
    return QString();
}

/** \brief Returns the hex encoded MD5 checksum of a file. */
QByteArray fileMd5(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray fileData = file.readAll();
        QByteArray hashData = QCryptographicHash::hash(fileData, QCryptographicHash::Md5);
        return hashData.toHex();
    }
    return QByteArray();
}

/** \brief Returns the hex encoded SHA1 checksum of a file. */
QByteArray fileSha1(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray fileData = file.readAll();
        QByteArray hashData = QCryptographicHash::hash(fileData, QCryptographicHash::Sha1);
        return hashData.toHex();
    }
    return QByteArray();
}

/** \brief Returns the SHA256 checksum of a file. */
QByteArray fileSha256(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray fileData = file.readAll();
        QByteArray hashData = QCryptographicHash::hash(fileData, QCryptographicHash::Sha256);
        return hashData.toHex();
    }
    return QByteArray();
}

QString humanReadableFileSize(qint64 size)
{
     float num = size;
     QStringList list;
     list << "KB" << "MB" << "GB" << "TB";
     QStringListIterator i(list);
     QString unit("bytes");
     while(num >= 1024.0 && i.hasNext()) {
         unit = i.next();
         num /= 1024.0;
     }
     return QString("%1 %2").arg(QString().setNum(num,'f',2)).arg(unit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////   MESSAGEBOXES FUNCTIONS   //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Creates an informative messagebox.
  \code
#include <utils/global.h>
[...]
    Utils::informativeMessageBox(...);
\endcode
**/
void informativeMessageBox(const QString &text, const QString &infoText, const QString &detail, const QString &title)
{
    QWidget *parent = qApp->activeWindow();
    QMessageBox mb(parent);
    mb.setWindowModality(Qt::WindowModal);
    mb.setIcon(QMessageBox::Information);
    if (title.isEmpty())
        mb.setWindowTitle(qApp->applicationName());
    else
        mb.setWindowTitle(title);
    mb.setText(text);
    mb.setInformativeText(infoText);
    if (!detail.isEmpty()) {
        if (Qt::mightBeRichText(detail)) {
            QTextDocument doc;
            doc.setHtml(detail);
            mb.setDetailedText(doc.toPlainText());
        } else {
            mb.setDetailedText(detail);
        }
    }
    mb.setStandardButtons(QMessageBox::Ok);
    mb.setDefaultButton(QMessageBox::Ok);
    mb.exec();
    qApp->setActiveWindow(parent);
}

/** \brief Creates a warning messagebox.
  \code
#include <utils/global.h>
[...]
    Utils::warningMessageBox(...);
\endcode
**/
void warningMessageBox(const QString &text, const QString &infoText, const QString &detail, const QString &title)
{
    LOG_FOR("Warning Dialog", infoText);
    if (!qobject_cast<QApplication*>(qApp)) {  // Equals to not GUI app
        qWarning() << "  * Text:" << text << "\n  * detail" << detail << "\n  * title:" << title;
        return;
    }
    QWidget *parent = qApp->activeWindow();
    QMessageBox mb(parent);
    mb.setWindowModality(Qt::WindowModal);
    mb.setIcon(QMessageBox::Warning);
    if (title.isEmpty())
        mb.setWindowTitle(qApp->applicationName());
    else
        mb.setWindowTitle(title);
    mb.setText(text);
    mb.setInformativeText(infoText);
    if (!detail.isEmpty()) {
        if (Qt::mightBeRichText(detail)) {
            QTextDocument doc;
            doc.setHtml(detail);
            mb.setDetailedText(doc.toPlainText());
        } else {
            mb.setDetailedText(detail);
        }
    }
    mb.setStandardButtons(QMessageBox::Ok);
    mb.setDefaultButton(QMessageBox::Ok);
    mb.exec();
    qApp->setActiveWindow(parent);
}

/** \brief Creates a messagebox with yes / no. Return true if user clicked yes.
  \code
#include <utils/global.h>
[...]
    bool yes = Utils::yesnoMessageBox(...);
    if (yes) {
        // yes clicked
    } else {
        // no clicked
    }
\endcode
**/
bool yesNoMessageBox(const QString &text, const QString&infoText, const QString&detail, const QString &title, const QPixmap &icon)
{
    QWidget *parent = qApp->activeWindow();
    QMessageBox mb(parent);
    mb.setWindowModality(Qt::WindowModal);
    mb.setWindowFlags(mb.windowFlags() & ~Qt::WindowCloseButtonHint);
    if (icon.isNull()) {
        mb.setIcon(QMessageBox::Question);
    } else {
        mb.setIconPixmap(icon);
    }
    if (title.isEmpty())
        mb.setWindowTitle(qApp->applicationName());
    else
        mb.setWindowTitle(title);
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    mb.setDefaultButton(QMessageBox::Yes);
    mb.setText(text);
    mb.setInformativeText(infoText);
    if (!detail.isEmpty()) {
        if (Qt::mightBeRichText(detail)) {
            QTextDocument doc;
            doc.setHtml(detail);
            mb.setDetailedText(doc.toPlainText());
        } else {
            mb.setDetailedText(detail);
        }
    }
    int r = mb.exec();
    qApp->setActiveWindow(parent);
    if (r==QMessageBox::Yes)
        return true;
    return false;
}

/**
  \brief Creates a messagebox with multiple buttons.
  Return -1 if dialog was cancelled, or the index of the button into the stringlist.
  \code
#include <utils/global.h>
[...]
    QStringList buttonText;
    buttonText << "1" << "2" << "3";
    int r = Utils::withButtonMessageBox(..., buttonText, ...);
    switch (r) {
    case -1:
        // Cancelled
        break;
    case 0:
        // Button "1"
        break;
    case 1:
        // Button "2"
        break;
    case 2:
        // Button "3"
        break;
    }
\endcode

**/
int withButtonsMessageBox(const QString &text, const QString&infoText, const QString&detail, const QStringList &buttonsText, const QString &title, bool withCancelButton)
{
    QWidget *parent = qApp->activeWindow();
    QMessageBox mb(parent);
    mb.setWindowModality(Qt::WindowModal);
    mb.setIcon(QMessageBox::Question);
    if (title.isEmpty())
        mb.setWindowTitle(qApp->applicationName());
    else
        mb.setWindowTitle(title);
    mb.setText(text);
    mb.setInformativeText(infoText);
    if (!detail.isEmpty()) {
        if (Qt::mightBeRichText(detail)) {
            QTextDocument doc;
            doc.setHtml(detail);
            mb.setDetailedText(doc.toPlainText());
        } else {
            mb.setDetailedText(detail);
        }
    }
    QList<QPushButton *> buttons;
    foreach(const QString &s, buttonsText) {
        buttons << mb.addButton(s, QMessageBox::YesRole);
    }
    if (withCancelButton) {
        buttons << mb.addButton(QApplication::translate("Utils", "Cancel"), QMessageBox::RejectRole);
    }
    mb.setDefaultButton(buttons.at(0));
    int r = mb.exec();
    qApp->setActiveWindow(parent);
    if (r==buttonsText.count())
        return -1;
    return buttons.indexOf(static_cast<QPushButton*>(mb.clickedButton()));
}

/**
  \brief Creates a messagebox with multiple standard buttons.
  Return the standard button selected.
  \code
#include <utils/global.h>
[...]
    QMessageBox::StandardButtons buts;
    buts = QMessageBox::Ok | QMessageBox::Save | QMessageBox::Cancel;
    int r = Utils::withButtonMessageBox(..., buts, ...);
    switch (r) {
    case QMessageBox::Ok:
        break;
    case QMessageBox::Save:
        break;
    case QMessageBox::Cancel:
        break;
    }
\endcode

**/
int withButtonsMessageBox(const QString &text, const QString&infoText, const QString&detail, QMessageBox::StandardButtons buts, QMessageBox::StandardButton defaultButton, const QString &title)
{
    QWidget *parent = qApp->activeWindow();
    QMessageBox mb(parent);
    mb.setWindowModality(Qt::WindowModal);
    mb.setIcon(QMessageBox::Question);
    if (title.isEmpty())
        mb.setWindowTitle(qApp->applicationName());
    else
        mb.setWindowTitle(title);
    mb.setText(text);
    mb.setInformativeText(infoText);
    if (!detail.isEmpty()) {
        if (Qt::mightBeRichText(detail)) {
            QTextDocument doc;
            doc.setHtml(detail);
            mb.setDetailedText(doc.toPlainText());
        } else {
            mb.setDetailedText(detail);
        }
    }
    mb.setStandardButtons(buts);
    mb.setDefaultButton(defaultButton);
    int r = mb.exec();
    qApp->setActiveWindow(parent);
    return r;
}

/** \brief Creates a messagebox with Ok / Cancel. Return true if user clicked ok. **/
bool okCancelMessageBox(const QString &text, const QString&infoText, const QString&detail, const QString &title)
{
    QWidget *parent = qApp->activeWindow();
    QMessageBox mb(parent);
    mb.setWindowModality(Qt::WindowModal);
    mb.setIcon(QMessageBox::Question);
    if (title.isEmpty())
        mb.setWindowTitle(qApp->applicationName());
    else
        mb.setWindowTitle(title);
    mb.setText(text);
    mb.setInformativeText(infoText);
    if (!detail.isEmpty()) {
        if (Qt::mightBeRichText(detail)) {
            QTextDocument doc;
            doc.setHtml(detail);
            mb.setDetailedText(doc.toPlainText());
        } else {
            mb.setDetailedText(detail);
        }
    }
    mb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    mb.setDefaultButton(QMessageBox::Ok);
    int r = mb.exec();
    qApp->setActiveWindow(parent);
    return (r==QMessageBox::Ok);
}

/** \brief Creates a messagebox for non available function.  **/
bool functionNotAvailableMessageBox(const QString &functionText)
{
    informativeMessageBox(functionText,
                           QCoreApplication::translate("Utils", "This function is not available in this version."),
                           QCoreApplication::translate("Utils", "You can send an email to developers and explain your difficulties: dev@freehealth.io")
                        );
//                           .arg(qApp->organizationDomain()));
//                         .arg(tkSettings::instance()->path(tkSettings::WebSiteUrl)));
    return true;
}

/** \brief Shows a full screen quick debug dialog that shows each string of the list inside a textbrowser */
void quickDebugDialog(const QStringList &texts)
{
    QDialog *dlg = new QDialog();
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    QGridLayout *grid = new QGridLayout(dlg);

    foreach(const QString &s, texts) {
        QTextBrowser *f = new QTextBrowser(dlg);
        if (Qt::mightBeRichText(s))
            f->setHtml(s);
        else
            f->setPlainText(s);
        grid->addWidget(f);
    }
    grid->addWidget(buttonBox);
    dlg->connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    setFullScreen(dlg,true);
    dlg->exec();
    delete buttonBox;
    delete dlg;
}

/** \brief Shows a default dialog with \e license terms and a small \e message. */
bool defaultLicenseAgreementDialog(const QString &message, Utils::LicenseTerms::AvailableLicense license)
{
    QDialog dlg;
    QGridLayout layout(&dlg);
    QDialogButtonBox buttonBox(QDialogButtonBox::Yes | QDialogButtonBox::No);
    QTextBrowser tbrowse(&dlg);
    tbrowse.setReadOnly(true);
    QLabel appname(&dlg);
    // Application name
    if (qApp->applicationName().isEmpty()) {
        dlg.setWindowTitle(QCoreApplication::translate("Utils", "License agreement acceptation"));
        appname.setText(QString("<b>%1</b>").arg(QCoreApplication::translate("Utils", "License agreement acceptation")));
    } else {
        dlg.setWindowTitle(qApp->applicationName());
        appname.setText(QString("<b>%1</b>").arg(qApp->applicationName()));
    }
    appname.setAlignment(Qt::AlignCenter);
    // Message
    QLabel centered;
    if (!message.isEmpty()) {
        centered.setText(message);
    } else {
        centered.setText(QCoreApplication::translate("Utils", "<b>Before you can use this software, you must agree its license terms</b>"));
    }
    QFont bold;
    bold.setBold(true);
    centered.setFont(bold);
    centered.setAlignment(Qt::AlignCenter);
    tbrowse.setText(Utils::LicenseTerms::getTranslatedLicenseTerms(license));
    // Question yes/no
    QLabel question(QCoreApplication::translate("Utils", "Do you agree these terms?"));
    layout.addWidget(&appname);
    layout.addWidget(&centered);
    layout.addWidget(&tbrowse);
    layout.addWidget(&question);
    layout.addWidget(&buttonBox);
    dlg.connect(&buttonBox, SIGNAL(accepted()), &dlg, SLOT(accept()));
    dlg.connect(&buttonBox, SIGNAL(rejected()), &dlg, SLOT(reject()));
    dlg.show();
    qApp->setActiveWindow(&dlg);
    dlg.activateWindow();
    dlg.raise();
    if (dlg.exec()==QDialog::Accepted)
        return true;
    return false;
}

/** \brief Creates a Dialog for simple user's input.  **/
QString askUser(const QString &title, const QString &question)
{
    bool ok;
    QString text = QInputDialog::getText(qApp->activeWindow(), title, question, QLineEdit::Normal, "", &ok);
    if (ok)
        return text;
    return QString();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////   WIDGETS FUNCTIONS   /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Center the widget into the desktop. **/
void centerWidget(QWidget *win, QWidget *reference)
{
    if (!win)
        return;
    QPoint center;
    // FIXME: buggy with multi-screen
    if (!reference) {
        // try to find the mainwindow
        QMainWindow *win = 0;
        QWidgetList list = qApp->topLevelWidgets();
        for(int i = 0; i < list.count(); ++i) {
            win = qobject_cast<QMainWindow*>(list.at(i));
            if (win && win->isVisible()) {
                break;
            }
        }
        if (win)
            reference = win;
        else
            reference = qApp->desktop();
    }
    center = reference->rect().center() + reference->pos();
    QRect rect = win->rect();
    rect.moveCenter(center);
    win->move(rect.topLeft());
}

/** \brief Switch widget to fullscreen/non fullscreen. **/
void setFullScreen(QWidget* win, bool on)
{
    // TODO: code with Qt::WindowFullScreen is buggy under mac os 10.6.8 -> windows is not visible
    if (win->isFullScreen()==on)
        return;

    if (on) {
        win->setWindowState(win->windowState() | Qt::WindowMaximized);
        LOG_FOR("Utils", QCoreApplication::translate("Utils", "%1 is now in fullScreen Mode.").arg(win->objectName()));
        //statusBar()->hide();
        //menuBar()->hide();
    } else {
        win->setWindowState(win->windowState() & ~Qt::WindowMaximized);
        LOG_FOR("Utils", QCoreApplication::translate("Utils", "%1 is now in non fullScreen Mode.").arg(win->objectName()));
        //menuBar()->show();
        //statusBar()->show();
    }
}

void resizeAndCenter(QWidget *widget, QWidget *reference)
{
    // resize windows
    QWidget *ref = reference;
    if (!reference)
        ref = qApp->activeWindow();
    if (!ref) {
        widget->adjustSize();
        return;
    }
    QSize size = ref->size();
    size = QSize(size.width()*0.9, size.height()*0.9);
    widget->resize(size);
    // recenter window
    centerWidget(widget, ref);
}

/** \brief Toggle the EchoMode property of a QLineEdit between Normal and Password. */
void switchEchoMode(QLineEdit * l)
{
    if (l->echoMode() == QLineEdit::Normal)
        l->setEchoMode(QLineEdit::Password);
    else
        l->setEchoMode(QLineEdit::Normal);
}

/** Rounds the \e date to the number of minutes \e minutesRound. */
QDateTime roundDateTime(const QDateTime &date, const int minutesRound)
{
    Q_ASSERT(minutesRound > 0);
    if (date.isNull())
        return QDateTime();
    if (date.time().minute() % minutesRound == 0
            && date.time().second() == 0
            && date.time().msec() == 0)
        return QDateTime(date.date(), QTime(date.time().hour(), date.time().minute(), 0));
    QDateTime dt = QDateTime(date.date(), QTime(date.time().hour(), date.time().minute(), 0));
    int minToRound = dt.time().minute() % minutesRound;
    dt = dt.addSecs((minutesRound - minToRound)*60);
    return dt;
}

bool inRange(const int min, const int max, const int value)
{
    return ((value >= min) && (value <= max));
}

bool inRange(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QModelIndex &value)
{
    // row in range ?
    if (!inRange(topLeft.row(), bottomRight.row(), value.row()))
        return false;
    int min = (topLeft.row() * 1000) + topLeft.column();
    int max = (bottomRight.row() * 1000) + bottomRight.column();
    int val = (value.row() * 1000) + value.column();
    return inRange(min, max, val);
}

/** Return the ISO-3166-1 2-char code of the \e country */
QString countryToIso(QLocale::Country country)
{
    if (country == QLocale::AnyCountry)
        return QString();

    QString code;
    code.resize(2);
    const unsigned char *c = two_letter_country_code_list + 2 * (uint(country));
    code[0] = ushort(c[0]);
    code[1] = ushort(c[1]);
    return code;
}

/*!
 * \brief Takes  \e country and returns a localized string using QLocale::countryToString()
 *
 * \param country QString an ISO-3166-1 2-char code of the country
 * \returns a localized string for the given code using QLocale::countryToString()
 */
QString countryIsoToName(const QString &country)
{
    if (country.size() != 2)
        return QString();
    // TODO: maybe remove all this code and do just
    // return QLocale::countryToString(countryIsoToCountry(country));
    // for code duplication decrease
    QString t;
    t.resize(2);
    int c = 2;
    int max = sizeof(two_letter_country_code_list)/sizeof (*two_letter_country_code_list);
    int i = c;
    while (i < max) {
        const unsigned char *c = two_letter_country_code_list + i;
        t[0] = ushort(c[0]);
        t[1] = ushort(c[1]);
        if (t.compare(country, Qt::CaseInsensitive)==0) {
            return QLocale::countryToString(QLocale::Country(i/2));
        }
        ++i;
        ++i;
    }
    return QString();
}

/*!
 * \brief Maps an ISO string to Qt's internal Country code.
 * \param country an ISO-3166-1 2-char string
 * \returns a QLocale::Country code for the given \e country
 */
QLocale::Country countryIsoToCountry(const QString &country) {
    if (country.size() != 2)
        return QLocale::AnyCountry;
    QString t;
    t.resize(2);
    int c = 2;
    int max = sizeof(two_letter_country_code_list)/sizeof (*two_letter_country_code_list);
    int i = c;
    while (i < max) {
        const unsigned char *c = two_letter_country_code_list + i;
        t[0] = ushort(c[0]);
        t[1] = ushort(c[1]);
        if (t.compare(country, Qt::CaseInsensitive) == 0) {
            return QLocale::Country(i/2);
        }
        ++i;
        ++i;
    }
    return QLocale::AnyCountry;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////   HTML FUNCTIONS   //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Extract the html body content and return it.
 * The body can will be replaced by default by a P tag but using \e replaceBodyTagByParagraphTag
 * you can select a null replacement (setting \e replaceBodyTagByParagraphTag to false - in
 * this case all css content of the body tag will be lost).
 *
 * \code
 * QString body = Utils::htmlBodyContent("<html><header></header><body style=\"bodyStyle;\">BODY <b>CONTENT</b></body></html>");
 * // body = "<p style=\"bodyStyle;\"><b>BODY CONTENT</b></p>";
 * \endcode
 *
 * \code
 * QString body = Utils::htmlBodyContent("<html><header></header><body style=\"bodyStyle;\">BODY <b>CONTENT</b></body></html>", false);
 * // body = "<b>BODY CONTENT</b>";
 * \endcode
 *
 * \warning This function will create a new HTML paragraph with the body style, including the body content.
 * If there is no body tag in the given argument, the function returns the full text.
 */
QString htmlBodyContent(const QString &fullHtml, bool replaceBodyTagByParagraphTag)
{
    if (fullHtml.isEmpty())
        return QString::null;
    // Catch body tag
    int beg = fullHtml.indexOf("<body");
    if (beg == -1)
        return fullHtml;
    // Extract the body css style
    beg += 5;
    int end = fullHtml.indexOf(">", beg);
    QString style = fullHtml.mid(beg, end-beg).trimmed();
    // Extract the body content
    beg = end + 1;
    end = fullHtml.indexOf("</body>", beg);

    // no </body> found?
    if (end < beg)
        end = fullHtml.length();

    if (replaceBodyTagByParagraphTag)
        return QString("<%1%2>%3</%1>")
                .arg("p")
                .arg(style.isEmpty()? "" : QString(" %1").arg(style))
                .arg(fullHtml.mid(beg, end-beg));
    return fullHtml.mid(beg, end-beg);
}

/**
 * Match all CSS style declaration, remove it from the HTML code and return it.
 * \warning The \e fullHtml will be modified.
*/
QString htmlTakeAllCssContent(QString &fullHtml)
{
    if (fullHtml.isEmpty())
        return QString::null;
    QString css;
    typedef QPair<int,int> pairs;
    QList<pairs> removalIndexes; // begin, end
    int begin = 0;
    while (begin >= 0) {
        begin = fullHtml.indexOf("<style", begin);
        if (begin != -1) {
            int end = fullHtml.indexOf("</style>", begin);
            if (end != -1) {
                // Found a block <style ...> ...css content... </style>
                end += 8;
                removalIndexes << pairs(begin, end);
                css += fullHtml.mid(begin, end-begin);
            } else {
                // Found a file declaration? <style .../>
                // TODO: code this
                end = fullHtml.indexOf("/>", begin);
                Q_ASSERT(false); // "CSS extraction from file declaration not yet implemented"
            }
            begin += end;
        }
    }

    // remove found css in reverse order
    for (int i = removalIndexes.count() - 1; i > -1; --i) {
        const pairs &indexes = removalIndexes.at(i);
        fullHtml = fullHtml.remove(indexes.first, indexes.second - indexes.first);
    }

    return css;
}

/** Replace all <p> </p> HTML tag with <div></div> */
QString htmlReplaceParagraphWithDiv(const QString &fullHtml)
{
    if (fullHtml.isEmpty())
        return QString::null;
    QString r = fullHtml;
    r = r.replace("<p>", "<div>");
    r = r.replace("<p ", "<div ");
    r = r.replace("</p>", "</div>");
    return r;
}

/**
 * Removes all links tags, but not the content of the link, in an HTML content \e fullHtml.
 * \code
 *   <a *>this is not removed</a>  -> this is not removed
 * \endcode
 */
QString htmlRemoveLinkTags(const QString &fullHtml)
{
    QString html = fullHtml;
    int begin = html.indexOf("<a ", 0, Qt::CaseInsensitive);
    while (begin != -1) {
        // remove the link
        int end = html.indexOf(">", begin);
        if (end != -1) {
            html = html.remove(begin, end-begin+1);
        }
        begin = html.indexOf("<a ", begin, Qt::CaseInsensitive);
    }
    html = html.remove("</a>", Qt::CaseInsensitive);
    return html;
}

/** \brief Return the CSS style for a font. **/
QString fontToHtml(const QFont &font, const QColor &color)
{
    QString style;
    style = QString("font-family:%1;").arg(font.family());
    style += QString("font-size:%1pt;").arg(font.pointSize());
    if (font.bold())
        style += "font-weight:bold;";
    else
        style += "font-weight:normal;";
    if (font.italic())
        style += "font-style:italic;";
    else
        style += "font-style:normal;";
    if (font.underline())
        style += "text-decoration:underline;";
    else
        style += "text-decoration:none;";
    if (color.isValid()) {
        style += QString("color:%1;").arg(color.name());
    }
    return style;
}

/** \brief Transform a Qt::Alignment to html equivalent. return things like : align="center" */
QString textAlignmentToHtml(const Qt::Alignment &align)
{
    QString toReturn;
    if (align & Qt::AlignCenter)
        toReturn = "center";
    else if (align & Qt::AlignJustify)
        toReturn = "justify";
    else if (align & Qt::AlignRight)
        toReturn = "right";
    else
        toReturn = "left";
    if (!toReturn.isEmpty()) {
        toReturn.prepend("align=\"");
        toReturn += "\" ";
    }
    return toReturn;
}

/**
 * \brief Assumes a better encoding of HTML files by replacing special
 * characters with the html code (é == &eacute;)
 **/
QString htmlReplaceAccents(const QString &html)
{
    if (html.isEmpty())
        return html;
    QString toReturn = html;
    QHash< QString, QString > accents;
    //    accents.insert(QString::fromUtf8("\""), "&quot;");
    accents.insert(QString::fromUtf8("é"), "&eacute;");
    accents.insert(QString(QChar(QChar::Nbsp)), "&nbsp;");

    accents.insert(QString::fromUtf8("è"), "&egrave;");
    accents.insert(QString::fromUtf8("à"), "&agrave;");
    accents.insert(QString::fromUtf8("ù"), "&ugrave;");

    accents.insert(QString::fromUtf8("ê"), "&ecirc;");
    accents.insert(QString::fromUtf8("â"), "&acirc;");
    accents.insert(QString::fromUtf8("î"), "&icirc;");
    accents.insert(QString::fromUtf8("ô"), "&ocirc;");
    accents.insert(QString::fromUtf8("û"), "&ucirc;");

    accents.insert(QString::fromUtf8("ä"), "&auml;");
    accents.insert(QString::fromUtf8("ë"), "&euml;");
    accents.insert(QString::fromUtf8("ï"), "&iuml;");
    accents.insert(QString::fromUtf8("ö"), "&ouml;");
    accents.insert(QString::fromUtf8("ü"), "&uuml;");

    accents.insert(QString::fromUtf8("œ"), "&oelig;");
    accents.insert(QString::fromUtf8("æ"), "&aelig;");
    accents.insert(QString::fromUtf8("ç"), "&ccedil;");

    accents.insert(QString::fromUtf8("ø"), "&oslash;");
    accents.insert(QString::fromUtf8("Ø"), "&Oslash;");

    accents.insert(QString::fromUtf8("É"), "&Eacute;");

    accents.insert(QString::fromUtf8("È"), "&Egrave;");
    accents.insert(QString::fromUtf8("À"), "&Agrave;");
    accents.insert(QString::fromUtf8("Ù"), "&Ugrave;");

    accents.insert(QString::fromUtf8("Ê"), "&Ecirc;");
    accents.insert(QString::fromUtf8("Â"), "&Acirc;");
    accents.insert(QString::fromUtf8("Î"), "&Icirc;");
    accents.insert(QString::fromUtf8("Ô"), "&Ocirc;");
    accents.insert(QString::fromUtf8("Û"), "&Ucirc;");

    accents.insert(QString::fromUtf8("Ä"), "&Auml;");
    accents.insert(QString::fromUtf8("Ë"), "&Euml;");
    accents.insert(QString::fromUtf8("Ï"), "&Iuml;");
    accents.insert(QString::fromUtf8("Ö"), "&Ouml;");
    accents.insert(QString::fromUtf8("Ü"), "&Uuml;");

    accents.insert(QString::fromUtf8("ã"), "&atilde;");
    accents.insert(QString::fromUtf8("õ"), "&otilde;");
    accents.insert(QString::fromUtf8("Ã"), "&Atilde;");
    accents.insert(QString::fromUtf8("Õ"), "&Otilde;");

    accents.insert(QString::fromUtf8("Œ"), "&OElig;");
    accents.insert(QString::fromUtf8("Æ"), "&AElig;");
    accents.insert(QString::fromUtf8("Ç"), "&Ccedil;");

    accents.insert(QString::fromUtf8("ø"), "&oslash;");
    accents.insert(QString::fromUtf8("Ø"), "&Oslash;");
    accents.insert(QString::fromUtf8("€"), "&#128;");
    accents.insert(QString::fromUtf8("¡"), "&iexcl;");

    accents.insert(QString::fromUtf8("¢"), "&cent;");
    accents.insert(QString::fromUtf8("£"), "&pound;");
    accents.insert(QString::fromUtf8("¤"), "&curren;");
    accents.insert(QString::fromUtf8("¥"), "&yen;");
    accents.insert(QString::fromUtf8("¦"), "&brvbar;");
    accents.insert(QString::fromUtf8("µ"), "&micro;");
    accents.insert(QString::fromUtf8("·"), "&middot;");
    accents.insert(QString::fromUtf8("»"), "&raquo;");
    accents.insert(QString::fromUtf8("«"), "&laquo;");
    accents.insert(QString::fromUtf8("¼"), "&frac14;");
    accents.insert(QString::fromUtf8("½"), "&frac12;");
    accents.insert(QString::fromUtf8("¾"), "&frac34;");
    accents.insert(QString::fromUtf8("¿"), "&iquest;");
    accents.insert(QString::fromUtf8("÷"), "&divide;");
    accents.insert(QString::fromUtf8("•"), "&#149;");
    accents.insert(QString::fromUtf8("©"), "&copy;");
    accents.insert(QString::fromUtf8("®"), "&reg;");
    accents.insert(QString::fromUtf8("™"), "&#153;");
    accents.insert(QString::fromUtf8("§"), "&sect;");
    accents.insert(QString::fromUtf8("…"), "&#133;");
    accents.insert(QString::fromUtf8("ˆ"), "&#136;");
    accents.insert(QString::fromUtf8("—"), "&mdash;");

    foreach(const QString &k, accents.keys()) {
        toReturn.replace(k, accents.value(k));
    }
    return toReturn;
}

/** Extract and return all CSS content href links */
QStringList htmlGetLinksToCssContent(const QString &html)
{
    // <link rel="stylesheet" href="../style/spstyl01.css" type="text/css">
    QStringList list;
    if (html.isEmpty())
        return list;

    int begin = 0;
    do {
        begin = html.indexOf("<link ", begin);
        if (begin == -1)
            break;
        int endTag = html.indexOf(">", begin+6);
        if (endTag == -1)
            break;
        QString content = html.mid(begin, endTag-begin);
        if (content.contains("css", Qt::CaseInsensitive)
                && content.contains("href", Qt::CaseInsensitive) ) {
            // Extract the href content
            int b = content.indexOf("href");
            b = content.indexOf("\"", b+4) + 1;
            int e = content.indexOf("\"", b);
            list << content.mid(b, e-b);
        }
        begin = endTag;
    } while (begin > 0);
    list.removeAll("");
    return list;
}

/** Capitalize the first letter of a string */
QString firstLetterUpperCase(const QString &s)
{
    if (s.isEmpty())
        return QString::null;
    QString tmp = s;
    tmp[0] = tmp[0].toUpper();
    return tmp;
}

/** Correct wrong accent UTF8 encoding (used by HPRIM code) */
QString correctTextAccentEncoding(const QString &text)
{
    QByteArray ba = text.toUtf8();
    QHash<QByteArray, QByteArray> conv;
    conv.insert(QByteArray("Å¡"), QByteArray("š"));
    conv.insert(QByteArray("Å'"), QByteArray("Œ"));
    conv.insert(QByteArray("Å\""), QByteArray("œ"));
    conv.insert(QByteArray("Å¸"), QByteArray("Ÿ"));
    conv.insert(QByteArray("Ã¿"), QByteArray("ÿ"));
    conv.insert(QByteArray("Ã€"), QByteArray("À"));
    QString s;
    s.append(QChar(195));
    s.append(QChar(160));
    conv.insert(s.toUtf8(), QByteArray("à"));
    conv.insert(QByteArray("Ã¡"), QByteArray("á"));
    conv.insert(QByteArray("Ã‚"), QByteArray("Â"));
    conv.insert(QByteArray("Ã¢"), QByteArray("â"));
    conv.insert(QByteArray("Ãƒ"), QByteArray("Ã"));
    conv.insert(QByteArray("Ã£"), QByteArray("ã"));
    conv.insert(QByteArray("Ã„"), QByteArray("Ä"));
    conv.insert(QByteArray("Ã¤"), QByteArray("ä"));
    conv.insert(QByteArray("Ã…"), QByteArray("Å"));
    conv.insert(QByteArray("Ã¥"), QByteArray("å"));
    conv.insert(QByteArray("Ã†"), QByteArray("Æ"));
    conv.insert(QByteArray("Ã¦"), QByteArray("æ"));
    conv.insert(QByteArray("Ã‡"), QByteArray("Ç"));
    conv.insert(QByteArray("Ã§"), QByteArray("ç"));
    conv.insert(QByteArray("Ãˆ"), QByteArray("È"));
    conv.insert(QByteArray("Ã¨"), QByteArray("è"));
    conv.insert(QByteArray("Ã‰"), QByteArray("É"));
    conv.insert(QByteArray("Ã©"), QByteArray("é"));
    conv.insert(QByteArray("ÃŠ"), QByteArray("Ê"));
    conv.insert(QByteArray("Ãª"), QByteArray("ê"));
    conv.insert(QByteArray("Ã‹"), QByteArray("Ë"));
    conv.insert(QByteArray("Ã«"), QByteArray("ë"));
    conv.insert(QByteArray("ÃŒ"), QByteArray("Ì"));
    conv.insert(QByteArray("Ã¬"), QByteArray("ì"));
    conv.insert(QByteArray("Ã"), QByteArray("Í"));
    conv.insert(QByteArray("Ã­"), QByteArray("í"));
    conv.insert(QByteArray("ÃŽ"), QByteArray("Î"));
    conv.insert(QByteArray("Ã®"), QByteArray("î"));
    conv.insert(QByteArray("Ã¯"), QByteArray("ï"));
    conv.insert(QByteArray("Ã"), QByteArray("Ð"));
    conv.insert(QByteArray("Ã°"), QByteArray("ð"));
    conv.insert(QByteArray("Ã'"), QByteArray("Ñ"));
    conv.insert(QByteArray("Ã±"), QByteArray("ñ"));
    conv.insert(QByteArray("Ã'"), QByteArray("Ò"));
    conv.insert(QByteArray("Ã²"), QByteArray("ò"));
    conv.insert(QByteArray("Ã\""), QByteArray("Ó"));
    conv.insert(QByteArray("Ã³"), QByteArray("ó"));
    conv.insert(QByteArray("Ã\""), QByteArray("Ô"));
    conv.insert(QByteArray("Ã´"), QByteArray("ô"));
    conv.insert(QByteArray("Ã•"), QByteArray("Õ"));
    conv.insert(QByteArray("Ãµ"), QByteArray("õ"));
    conv.insert(QByteArray("Ã–"), QByteArray("Ö"));
    conv.insert(QByteArray("Ã˜"), QByteArray("Ø"));
    conv.insert(QByteArray("Ã¸"), QByteArray("ø"));
    conv.insert(QByteArray("Ã™"), QByteArray("Ù"));
    conv.insert(QByteArray("Ã¹"), QByteArray("ù"));
    conv.insert(QByteArray("Ãš"), QByteArray("Ú"));
    conv.insert(QByteArray("Ãº"), QByteArray("ú"));
    conv.insert(QByteArray("Ã›"), QByteArray("Û"));
    conv.insert(QByteArray("Ã»"), QByteArray("û"));
    conv.insert(QByteArray("Ãœ"), QByteArray("Ü"));
    conv.insert(QByteArray("Ã¼"), QByteArray("ü"));
    conv.insert(QByteArray("Ã½"), QByteArray("ý"));
    conv.insert(QByteArray("Ãž"), QByteArray("Þ"));
    conv.insert(QByteArray("Ã¾"), QByteArray("þ"));
    conv.insert(QByteArray("ÃŸ"), QByteArray("ß"));
    conv.insert(QByteArray("Ã¶"), QByteArray("ö"));
//    conv.insert(QByteArray("Ã"), QByteArray("Ï"));
//    conv.insert(QByteArray("Å "), QByteArray("Š"));
//    conv.insert(QByteArray("Ã"), QByteArray("Á"));
//    conv.insert(QByteArray("Ã"), QByteArray("Ý"));
    QHash<QByteArray, QByteArray>::const_iterator it = conv.constBegin();
    while (it != conv.constEnd()) {
        ba = ba.replace(it.key(), it.value());
        ++it;
    }
    return QString::fromUtf8(ba);
}

/** Removes accent from a UTF8 only string */
QString removeAccents(const QString &text)
{
    if (text.isEmpty())
        return QString::null;
    QHash< QString, QString > accents;
    accents.insert(QString::fromUtf8("é"), "e");
    accents.insert(QString::fromUtf8("è"), "e");
    accents.insert(QString::fromUtf8("à"), "a");
    accents.insert(QString::fromUtf8("ù"), "u");

    accents.insert(QString::fromUtf8("ê"), "e");
    accents.insert(QString::fromUtf8("â"), "a");
    accents.insert(QString::fromUtf8("î"), "i");
    accents.insert(QString::fromUtf8("ô"), "o");
    accents.insert(QString::fromUtf8("û"), "u");

    accents.insert(QString::fromUtf8("ä"), "a");
    accents.insert(QString::fromUtf8("ë"), "e");
    accents.insert(QString::fromUtf8("ï"), "i");
    accents.insert(QString::fromUtf8("ö"), "o");
    accents.insert(QString::fromUtf8("ü"), "u");

    accents.insert(QString::fromUtf8("œ"), "oe");
    accents.insert(QString::fromUtf8("æ"), "ae");
    accents.insert(QString::fromUtf8("ç"), "c");

    accents.insert(QString::fromUtf8("ø"), "o");
    accents.insert(QString::fromUtf8("Ø"), "O");

    accents.insert(QString::fromUtf8("É"), "E");
    accents.insert(QString::fromUtf8("È"), "E");
    accents.insert(QString::fromUtf8("À"), "A");
    accents.insert(QString::fromUtf8("Ù"), "U");

    accents.insert(QString::fromUtf8("Â"), "A");
    accents.insert(QString::fromUtf8("Ê"), "E");
    accents.insert(QString::fromUtf8("Î"), "I");
    accents.insert(QString::fromUtf8("Ô"), "O");
    accents.insert(QString::fromUtf8("Û"), "U");

    accents.insert(QString::fromUtf8("Ä"), "A");
    accents.insert(QString::fromUtf8("Ë"), "E");
    accents.insert(QString::fromUtf8("Ï"), "I");
    accents.insert(QString::fromUtf8("Ö"), "O");
    accents.insert(QString::fromUtf8("Ü"), "U");

    accents.insert(QString::fromUtf8("Œ"), "OE");
    accents.insert(QString::fromUtf8("Æ"), "AE");
    accents.insert(QString::fromUtf8("Ç"), "C");

    accents.insert(QString::fromUtf8("ø"), "o");
    accents.insert(QString::fromUtf8("Ø"), "O");

    accents.insert(QString::fromUtf8("ã"), "a");
    accents.insert(QString::fromUtf8("õ"), "o");
    accents.insert(QString::fromUtf8("Ã"), "A");
    accents.insert(QString::fromUtf8("Õ"), "O");

    QString toReturn = text;
    foreach(const QString &k, accents.keys())
        toReturn = toReturn.replace(k, accents.value(k));

    return toReturn;
}

static inline bool isLineSplitter(const QChar &c)
{
    return (c == ' '
            || c =='/'
            || c ==','
            || c ==';'
            || c =='.'
            || c =='?'
            || c ==':'
            || c =='-');
}

/** Split a string to multiple lines, with a max \e lineLength char per lines */
QString lineWrapString(const QString &in, int lineLength)
{
    if (in.isEmpty())
        return QString::null;
    QString tempStr = in;
    int len = in.length();
    int pos = lineLength;

    while (pos < len-1) {
        // Here we are at the supposed linebreak
        int tempPos = pos;

        // Find line breaker char (space, divider, coma...) going backward
        while (!isLineSplitter(tempStr.at(tempPos)) && tempPos > 0) {
            tempPos--;
        }

        // Update the current position
        if (tempPos > 0)
            pos=tempPos;

        // Cut the string
        if (tempStr.at(tempPos)==' ') {
            tempStr.replace(tempPos, 1, '\n');
        } else {
            tempStr.insert(tempPos, '\n');
            ++len;
        }

        // Go to next supposed linebreak
        pos+=lineLength;
    }

    return tempStr;
}

/** Center a string inside a line using \e fill to fill empty space */
QString centerString(const QString &in, const QChar &fill, int lineSize)
{
    QString out;
    out.fill(fill, lineSize);
    int begin = lineSize/2 - in.size()/2;
    if (begin <= 0)
        return in;
    out = out.replace(begin, in.size(), in);
    return out;
}

/** Indent a string of \e lineIndent spaces */
QString indentString(const QString &in, int lineIndent)
{
    if (in.isEmpty())
        return QString::null;
    QString indent;
    indent = indent.fill(' ', lineIndent);
    QString correctedIn = in;
    correctedIn = correctedIn.replace("\n", QString("\n"+indent));
    return QString("%1%2").arg(indent).arg(correctedIn);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////   XML FUNCTIONS   //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
  \brief Create a simple Xml content with a \e mainTag and a hash \e data.
  You can specify the indentation of the resulting Xml.\n
  You can automatically encode to base64 the values of the tags.\n
  The \e mainTag represents the first englobing Xml tag of the output.\n
  The tags are added in an unordered way cause of the uses of the QHash.\n
  Usage :
  \code
      QHash<QString, QString> tag_value;
      tag_value.insert("Tag", "Value");
      tag_value.insert("Tag2", "Value2");
      QString xml = createXml("MyXmlFirstTag", tag_value, 4, true);
      // Result is :
      // <MyXmlFirstTag>
      //     <Tag>Base64OfValue</Tag>
      //     <Tag2>Base64OfValue2</Tag2>
      // </MyXmlFirstTag>
  \endcode
*/
QString createXml(const QString &mainTag, const QHash<QString,QString> &data, const int indent,const bool valueToBase64 )
{
    QDomDocument doc;
    QDomElement main = doc.createElement(mainTag);
    doc.appendChild(main);
    if (valueToBase64) {
        foreach(const QString &k, data.keys()) {
            QDomElement dataElement  = doc.createElement(k);
            main.appendChild(dataElement);
            if (!data.value(k).isEmpty()) {
                QDomText dataText = doc.createTextNode(data.value(k).toUtf8().toBase64());
                dataElement.appendChild(dataText);
            }
        }
    } else {
        foreach(const QString &k, data.keys()) {
            QDomElement dataElement  = doc.createElement(k);
            main.appendChild(dataElement);
            if (!data.value(k).isEmpty()) {
                QDomText dataText = doc.createTextNode(data.value(k));
                dataElement.appendChild(dataText);
            }
        }
    }
    return doc.toString(indent);
}

/**
  \brief Reads a Xml content. Content must be like the one produced by createXml(). The \e readData is cleared and filled.
  \sa createXml().
*/
bool readXml(const QString &xmlContent, const QString &generalTag, QHash<QString,QString> &readData, const bool valueFromBase64)
{
    if (!xmlContent.contains(generalTag)) {
        Utils::Log::addError("Utils::readXml",QString("Error while reading Xml: tag %1 not found").arg(generalTag),
                             __FILE__, __LINE__);
        return false;
    }
    readData.clear();

    QDomDocument doc;
    doc.setContent(xmlContent);
    QDomElement root = doc.documentElement();
    QDomElement paramElem = root.firstChildElement();

    if (valueFromBase64) {
        while (!paramElem.isNull()) {
            if (!paramElem.tagName().compare(generalTag, Qt::CaseInsensitive)) {
                paramElem = paramElem.nextSiblingElement();
                continue;
            }
            readData.insert(paramElem.tagName(), QByteArray::fromBase64(paramElem.text().trimmed().toUtf8()));
            paramElem = paramElem.nextSiblingElement();
        }
    } else {
        while (!paramElem.isNull()) {
            if (!paramElem.tagName().compare(generalTag, Qt::CaseInsensitive)) {
                paramElem = paramElem.nextSiblingElement();
                continue;
            }
            readData.insert(paramElem.tagName(), paramElem.text().trimmed().toUtf8());
            paramElem = paramElem.nextSiblingElement();
        }
    }

    return true;
}

QString xmlRead(const QDomElement &father, const QString &name, const QString &defaultValue)
{
    QDomElement elem = father.firstChildElement(name);

    if (elem.isNull())
        return defaultValue;

    return elem.text();
}

QString xmlRead(const QDomElement &father, const QString &name, const char *defaultValue)
{
    QString defaultStr(defaultValue);
    return Utils::xmlRead(father, name, defaultStr);
}

int xmlRead(const QDomElement &father, const QString &name, const int defaultValue)
{
    QString defaultStr = QString::number(defaultValue);
    QString strValue = Utils::xmlRead(father, name, defaultStr);
    bool ok;
    int val = strValue.toInt(&ok);
    if (ok)
        return val;
    else
        return defaultValue;
}

int xmlRead(const QDomElement &father, const QString &name, const long int defaultValue)
{
    QString defaultStr = QString::number(defaultValue);
    QString strValue = Utils::xmlRead(father, name, defaultStr);
    bool ok;
    long int val = strValue.toLong(&ok);
    if (ok)
        return val;
    else
        return defaultValue;
}

bool xmlRead(const QDomElement &father, const QString &name, const bool defaultValue)
{
    QString defaultStr = QString::number((int) defaultValue);
    QString strValue = Utils::xmlRead(father, name, defaultStr);
    bool ok;
    int val = strValue.toInt(&ok);
    if (ok)
        return val;
    else
        return defaultValue;
}

void xmlWrite(QDomElement &father, const QString &name, const QString &value)
{
    QDomDocument document = father.ownerDocument();

    QDomElement elem = document.createElement(name);
    father.appendChild(elem);

    QDomText t = document.createTextNode(value);
    elem.appendChild(t);
}

void xmlWrite(QDomElement &father, const QString &name, char *value)
{
    QString strValue(value);
    Utils::xmlWrite(father, name, strValue);
}

void xmlWrite(QDomElement &father, const QString &name, int value)
{
    QString valueStr = QString::number(value);
    Utils::xmlWrite(father, name, valueStr);
}

void xmlWrite(QDomElement &father, const QString &name, long int value)
{
    QString valueStr = QString::number(value);
    Utils::xmlWrite(father, name, valueStr);
}

void xmlWrite(QDomElement &father, const QString &name, bool value)
{
    QString valueStr = QString::number((int) value);
    Utils::xmlWrite(father, name, valueStr);
}

/**
 * Transform a QPixmap to a base 64 QByteArray
 * \sa pixmapFromBase64()
*/
QByteArray pixmapToBase64(const QPixmap &pixmap)
{
    QByteArray byteArray;
    if (pixmap.isNull())
        return byteArray;
    QBuffer buffer(&byteArray);
    if (!pixmap.save(&buffer, "PNG")) {
        LOG_ERROR_FOR("Global",
                      QString("Unable to transform QPixmap to base64 QByteArray. "
                              "Pixmap size: (%1,%2)")
                      .arg(pixmap.size().width())
                      .arg(pixmap.size().height()));
        return QByteArray();
    }
    return byteArray.toBase64();
}

/**
 * Transform a QPixmap to a base 64 QByteArray
 * \sa pixmapFromBase64()
*/
QPixmap pixmapFromBase64(const QByteArray &base64)
{
    QPixmap pix;
    if (base64.isEmpty())
        return pix;
    if (!pix.loadFromData(QByteArray::fromBase64(base64))) {
        LOG_ERROR_FOR("Global", "Unable to transform base64 QByteArray to QPixmap");
        return QPixmap();
    }
    return pix;
}

/** \brief Replace a token into a string. */
int replaceToken(QString &textToAnalyse, const QString &token, const QString &value)
{
    if (!textToAnalyse.contains(token))
        return 0;
    // replace all occurences of token: token must not contain [ and ]
    QString t = token;
    t.remove(Constants::TOKEN_OPEN);
    t.remove(Constants::TOKEN_CLOSE);
    int begin, end;
    begin = 0;
    end = 0;
    int tokenLength = token.length() + QString(Constants::TOKEN_OPEN).length() + QString(Constants::TOKEN_CLOSE).length();
    int toReturn = 0;
    while (true) {
        // Find '[TOKEN]'
        begin = textToAnalyse.indexOf(Constants::TOKEN_OPEN + token + Constants::TOKEN_CLOSE, begin);
        if (begin==-1)
            break;
        end = begin + tokenLength;
        // Find text before '[ BEFORE [TOKEN]]'
        int beforeBegin;
        beforeBegin = textToAnalyse.lastIndexOf(Constants::TOKEN_OPEN, begin - 1);
        // Find text after '[[TOKEN] AFTER ]'
        int afterEnd;
        afterEnd = textToAnalyse.indexOf(Constants::TOKEN_CLOSE, end);
        if ((beforeBegin==-1) || (afterEnd==-1)) {
            Utils::Log::addError("Utils", QApplication::translate("Utils", "Token replacement error (%1). Wrong number of parentheses.")
                                                                .arg(token + QString::number(beforeBegin)),
                                                                __FILE__, __LINE__);
            begin = end;
            continue;
        }
        // Replace TOKEN and manage AFTER and BEFORE
        if (value.isEmpty()) {
            textToAnalyse.remove(beforeBegin, afterEnd-beforeBegin+1);
            ++toReturn;
        } else {
            QString before = textToAnalyse.mid(beforeBegin, begin-beforeBegin);
            QString after = textToAnalyse.mid(end, afterEnd-end);
//            qWarning() << beforeBegin << begin << before << end << afterEnd << after;
            textToAnalyse.remove(afterEnd,1);
            textToAnalyse.replace(begin, end-begin, value);
            textToAnalyse.remove(beforeBegin,1);
            ++toReturn;
        }
    }
    return toReturn;
}

int replaceTokens(QString &textToAnalyse, const QHash<QString, QString> &tokens_values)
{
    if (tokens_values.isEmpty())
        return 0;
    int i = 0;
    foreach(const QString &tok, tokens_values.keys()) {
        i += replaceToken(textToAnalyse, tok, tokens_values.value(tok));
    }
    return i;
}

int replaceTokens(QString &textToAnalyse, const QHash<QString, QVariant> &tokens_values)
{
    if (tokens_values.isEmpty())
        return 0;
    int i = 0;
    foreach(const QString &tok, tokens_values.keys()) {
        i += replaceToken(textToAnalyse, tok, tokens_values.value(tok).toString());
    }
    return i;
}

/**
 * Test the internet connection capability, and return the first available configuration identifier.
 * This code needs Qt 4.7+
 * \sa QNetworkConfigurationManager::configurationFromIdentifier(const QString &identifier) const
 * \bug this function is inhibited due to a Qt bug on MacOs X 10.6 & others
*/
QString testInternetConnection()
{
//    QNetworkConfigurationManager mgr;
//    LOG_FOR("Utils", QString("Testing internet connection. ManagerIsOnline=%1").arg(mgr.isOnline()));
//    QList<QNetworkConfiguration> activeConfigs = mgr.allConfigurations(QNetworkConfiguration::Active);
//    foreach(const QNetworkConfiguration &config, activeConfigs) {
//        if (config.isValid() && config.type()==QNetworkConfiguration::InternetAccessPoint) {
//            LOG_FOR("Utils", QString("%1 %2 (valid: %3)").arg(config.name()).arg(config.type()).arg(config.isValid()));
//            if (mgr.isOnline()) {
//                LOG_FOR("Utils", QString("Internet connection found %1").arg(config.name()));
//                return config.name();
//            }
//        }
//    }
//    LOG_FOR("Utils", QString("No internet connection"));
    return QString("yes");
}

/**
 * Create a database/filename compatible UUID.
 * This code uses QUuid::createUuid().toString() with some string contraction.
 */
QString createUid()
{
    return QUuid::createUuid().toString().remove("-").remove("{").remove("}");
}

/**
 * Link all QAbstractItemModel signals of the first model \e model1 to the second model \e model2 signals.
 * The SQLModel specific signal are not connected (primeInsert...)
 */
void linkSignalsFromFirstModelToSecondModel(QAbstractItemModel *model1, QAbstractItemModel *model2, bool connectDataChanged)
{
    QObject::connect(model1, SIGNAL(columnsAboutToBeInserted(QModelIndex, int , int )), model2, SIGNAL(columnsAboutToBeInserted(QModelIndex, int , int )));
    QObject::connect(model1, SIGNAL(columnsAboutToBeMoved(QModelIndex, int , int , QModelIndex, int )), model2, SIGNAL(columnsAboutToBeMoved(QModelIndex, int , int , QModelIndex, int )));
    QObject::connect(model1, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int , int )), model2, SIGNAL( columnsAboutToBeRemoved(QModelIndex, int , int )));
    QObject::connect(model1, SIGNAL(columnsInserted(QModelIndex, int , int )), model2, SIGNAL(columnsInserted(QModelIndex, int , int )));
    QObject::connect(model1, SIGNAL(columnsMoved(QModelIndex, int , int , QModelIndex, int )), model2, SIGNAL(columnsMoved(QModelIndex, int , int , QModelIndex, int )));
    QObject::connect(model1, SIGNAL(columnsRemoved(QModelIndex, int , int )), model2, SIGNAL(columnsRemoved(QModelIndex, int , int )));
    if (connectDataChanged)
        QObject::connect(model1, SIGNAL(dataChanged(QModelIndex,QModelIndex)), model2, SIGNAL(dataChanged(QModelIndex,QModelIndex)));
    QObject::connect(model1, SIGNAL(headerDataChanged(Qt::Orientation, int, int)), model2, SIGNAL(headerDataChanged(Qt::Orientation, int, int)));
    QObject::connect(model1, SIGNAL(layoutAboutToBeChanged()), model2, SIGNAL(layoutAboutToBeChanged()));
    QObject::connect(model1, SIGNAL(layoutChanged()), model2, SIGNAL(layoutChanged()));
    QObject::connect(model1, SIGNAL(modelAboutToBeReset()), model2, SIGNAL(modelAboutToBeReset()));
    QObject::connect(model1, SIGNAL(modelReset()), model2, SIGNAL(modelReset()));
    QObject::connect(model1, SIGNAL(rowsAboutToBeInserted(QModelIndex, int , int )), model2, SIGNAL(rowsAboutToBeInserted(QModelIndex, int , int )));
    QObject::connect(model1, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int , QModelIndex, int)), model2, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int , QModelIndex, int)));
    QObject::connect(model1, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int , int )), model2, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int , int )));
    QObject::connect(model1, SIGNAL(rowsInserted(QModelIndex, int , int )), model2, SIGNAL(rowsInserted(QModelIndex, int , int )));
    QObject::connect(model1, SIGNAL(rowsMoved(QModelIndex, int , int , QModelIndex, int )), model2, SIGNAL(rowsMoved(QModelIndex, int , int , QModelIndex, int )));
    QObject::connect(model1, SIGNAL(rowsRemoved(QModelIndex, int , int )), model2, SIGNAL(rowsRemoved(QModelIndex, int , int )));
}

/** Removes duplicates from the QVector \e vector and return the resulting QVector. */
QVector<int> removeDuplicates(const QVector<int> &vector)
{
    QList<int> noDuplicates;
    foreach(int i, vector) {
        if (noDuplicates.contains(i))
            continue;
        noDuplicates << i;
    }
    return noDuplicates.toVector();
}

/** Removes duplicates from the QList \e list and return the resulting QList. */
QList<int> removeDuplicates(const QList<int> &list)
{
    QList<int> noDuplicates;
    foreach(int i, list) {
        if (noDuplicates.contains(i))
            continue;
        noDuplicates << i;
    }
    return noDuplicates;
}

} // End Utils
