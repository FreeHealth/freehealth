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
/**
  \class Core::Translators
  \brief This class is a QTranslator manager. All translations files are loaded using the pathToTranslations().

  In first, instanciate this class inside the core of the app.

  Then set the translation path to use with : setPathToTranslations().

  You can add new translations file using members addNewTranslator().
  - specifying filename only (eg : 'myTrans' the suffix ('_lang') is automatcally added (using QLocale::staticData).
  - specifying full path of the file.

  Use changeLangage() to automacally reload all translators with the right language.Toolkit manages its own translator.

  Get availables languages and locales with : availableLocales(), availableLanguages(), availableLocalesAndLanguage().
*/

#include "translators.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>

#include <QTranslator>
#include <QFileInfo>
#include <QDir>
#include <QLocale>
#include <QApplication>
#include <QLibraryInfo>

using namespace Core;

enum { WarnTranslatorsErrors = false };

QString Translators::m_PathToTranslations = "";
Translators * Translators::m_Instance = 0;

/** \brief Get the unique instance of Translators */
Translators *Translators::instance(QObject *parent)
{
    if (!m_Instance) {
        if (parent)
            m_Instance = new Translators(qApp);
        else
            m_Instance = new Translators(parent);
    }
    return m_Instance;
}

/** \brief Protected constructor. Call instance() to get the pointer to this class. */
Translators::Translators(QObject * parent)
          : QObject(parent)
{
    setObjectName("Translators");
    m_Translators.clear();
//    setPathToTranslations(qApp->applicationDirPath());
    m_Instance = this;
}

/** \brief Destructor */
Translators::~Translators()
{
}

/**
  \brief Defines the path to translations
  \sa tkSettings::Paths, tkSettings::setPath()
*/
bool Translators::setPathToTranslations(const QString & path)
{
    if (QDir(path).exists()) {
        m_PathToTranslations = QDir::cleanPath(path);
        LOG_FOR("Translators", Trans::ConstantTranslations::tkTr(Trans::Constants::SETTING_1_PATH_TO_2)
                .arg(Trans::ConstantTranslations::tkTr(Trans::Constants::TRANSLATORS_TEXT),
                     QDir::cleanPath(path)));
        return true;
    }
    if (WarnTranslatorsErrors) {
        LOG_ERROR_FOR("Translators", Trans::ConstantTranslations::tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS)
                      .arg(QDir::cleanPath(path)));
    }
    return false;
}

/** \brief Returns the path the actual translations */
QString Translators::pathToTranslations()
{
    return m_PathToTranslations;
}

/**
  \brief change the default langage for the application and reload all translators.
  You can retreive langage using QLocale().name().left(2) anywhere in the program.
*/
void Translators::changeLanguage(const QString &lang)
{
    if (WarnTranslatorsErrors)
        qWarning() << "Translators::changeLanguage" << lang;
    QString l = lang.left(2);
    QLocale::setDefault(l);

    // load translations
//    if (l.toLower()=="en") {
//        foreach(QTranslator *t, m_Translators.values())
//            qApp->removeTranslator(t);
//    } else {
        foreach(const QString & fileMask, m_Translators.keys()) {
            QFileInfo f(fileMask);
            // this automatically send a QEvent::LanguageChange
            QString path = "";
            if (fileMask.contains(QDir::separator()))
                path = f.absolutePath();
            else
                path = m_PathToTranslations;

            if (!m_Translators[fileMask]->load(f.fileName() + "_" + lang, path)) {
                if (WarnTranslatorsErrors)
                    LOG_ERROR(tr("Can not load %1, path: %2").arg(f.fileName() + "_" + lang , path));
            } else {
                if (WarnTranslatorsErrors) {
                    LOG(Trans::ConstantTranslations::tkTr(Trans::Constants::FILE_1_LOADED).arg(f.fileName() + "_" + lang));
                }
            }
        }
//    }

    Q_EMIT languageChanged();
}

void Translators::changeLanguage(QLocale::Language lang)
{
    if (WarnTranslatorsErrors)
        qWarning() << "Translators::changeLanguage" << lang;
    changeLanguage(QLocale(lang).name().left(2));
}

/*!
 * \brief Add a translator to the known application translators. It can be for the main application or
 * for plugins.
 * This member will automatically add the translator located at fileMask into the QApplication translators.
 * \param fileMask: full path to qm file. Like this: "/path/to/qm/file" without "_en.qm" for example.
 * \return true if all gone ok, false in the other case
 */
bool Translators::addNewTranslator(const QString & fileMask, bool fromDefaultPath)
{
    QTranslator *t = new QTranslator(qApp);
    QString lang = QLocale().name().left(2).toLower();
    QString path;
    if (fileMask.compare("qt", Qt::CaseInsensitive) == 0) {
        if (Utils::isLinuxIntegratedCompilation() || Utils::isRunningOnLinux() || Utils::isRunningOnFreebsd())
            path = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    }
    QFileInfo file(fileMask);

    // manage path
    if (path.isEmpty()) {
        if (fromDefaultPath)
            path = m_PathToTranslations;
        else
            path = file.absolutePath();
    }

    // if translator loads
    if (t->load(file.fileName() + "_" + lang, path)) {
        // add it to the map and the application
        if (!m_Translators.contains(QDir::cleanPath(fileMask))) {
            m_Translators.insert(QDir::cleanPath(fileMask) , t);
            qApp->installTranslator(t);
            if (WarnTranslatorsErrors) {
                LOG(tr("Add Translator %1.").arg(file.fileName() + "_" + lang));
            }
        }
        return true;
    }

    if (WarnTranslatorsErrors) {
        LOG_ERROR(tr("WARNING: %1 can not be loaded or is already loaded.").arg(file.absoluteFilePath() + "_" + lang));
    }
    delete t;
    return false;
}

/** \brief Adds a new translator from the \e path and with the template filename \e fileTemplate */
bool Translators::addNewTranslator(const QString & path, const QString & fileTemplate)
{
    return addNewTranslator(path + QDir::separator() + fileTemplate, false);
}

/** \brief Returns the availables translated locales for the application. */
QStringList Translators::availableLocales()
{
    return availableLocalesAndLanguages().keys();
}

/** \brief Returns the available translated languages for the application. It is based on the Qt qm files. */
QStringList Translators::availableLanguages()
{
    return availableLocalesAndLanguages().values();
}

/** \brief Returns the available translated languages for the application. It is based on the Qt qm files. */
QMap<QString, QString> Translators::availableLocalesAndLanguages()
{
    static QMap<QString, QString> toReturn;
    if (!toReturn.isEmpty())
        return toReturn;

    toReturn.insert("en", "English");

    if (m_PathToTranslations.isEmpty())
        return toReturn;

    QDir dir(m_PathToTranslations);
    QStringList fileNames = dir.entryList(QStringList() << QString("%1_*.qm").arg(Trans::Constants::CONSTANTS_TRANSLATOR_NAME));
    foreach(QString s, fileNames) {
        QString locale = s;
        locale.remove(0, locale.lastIndexOf('_') + 1);
        locale.truncate(locale.lastIndexOf('.'));
        QTranslator translator;
        translator.load(s, m_PathToTranslations);
        QString lang = translator.translate(Trans::Constants::CONSTANTS_TR_CONTEXT, Trans::Constants::ENGLISH);
        toReturn.insert(locale, lang);
    }
    return toReturn;
}
