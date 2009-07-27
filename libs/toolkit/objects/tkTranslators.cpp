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

/**
  \class tkTranslators
  \brief This class is a QTranslator manager. All translations files are loaded using the pathToTranslations().

  In first, instanciate this class inside the core of the app.

  Then set the translation path to use with : setPathToTranslations().

  You can add new translations file using members addNewTranslator().
  - specifying filename only (eg : 'myTrans' the suffix ('_lang') is automatcally added (using QLocale::staticDatas).
  - specifying full path of the file.

  Use changeLangage() to automacally reload all translators with the right language.Toolkit manages its own translator.

  Get availables languages and locales with : availableLocales(), availableLanguages(), availableLocalesAndLanguage().

  \ingroup toolkit
  \ingroup object_toolkit
*/

#include "tkTranslators.h"
#include "tkLog.h"
#include <tkConstantTranslations.h>

#include <QTranslator>
#include <QFileInfo>
#include <QDir>
#include <QLocale>
#include <QApplication>

Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS

QString tkTranslators::m_PathToTranslations = "";
tkTranslators * tkTranslators::m_Instance = 0;

/** \brief Get the unique instance of tkTranslators */
tkTranslators *tkTranslators::instance( QObject *parent )
{
    if (!m_Instance) {
        if (parent)
            m_Instance = new tkTranslators(qApp);
        else
            m_Instance = new tkTranslators(parent);
    }
    return m_Instance;
}

/** \brief Protected constructor. Call instance() to get the pointer to this class. */
tkTranslators::tkTranslators( QObject * parent )
          : QObject( parent )
{
    setObjectName( "tkTranslators" );
    m_Translators.clear();
    setPathToTranslations( qApp->applicationDirPath() );
    m_Instance = this;
}

/** \brief Destructor */
tkTranslators::~tkTranslators()
{
}

/**
  \brief Defines the path to translations
  \sa tkSettings::Paths, tkSettings::setPath()
*/
bool tkTranslators::setPathToTranslations( const QString & path )
{
    if ( QDir( path ).exists() ) {
        m_PathToTranslations = QDir::cleanPath( path );
        tkLog::addMessage( "tkTranslators", tkTr(SETTING_1_PATH_TO_2).arg( tkTr(TRANSLATORS_TEXT), QDir::cleanPath(path) ) );
        return true;
    } else {
        tkLog::addError( "tkTranslators", tkTr(PATH_1_DOESNOT_EXISTS).arg( QDir::cleanPath( path ) ) );
        return false;
    }
}

/** \brief Returns the path the actual translations */
QString tkTranslators::pathToTranslations()
{
    return m_PathToTranslations;
}

/**
  \brief change the default langage for the application and reload all translators.
  You can retreive langage using QLocale().name().left( 2 ) anywhere in the program.
*/
void tkTranslators::changeLanguage( const QString & lang )
{
    QString l = lang.left(2);
    QLocale::setDefault(l);

    // load translations
//    if (l.toLower()=="en") {
//        foreach(QTranslator *t, m_Translators.values())
//            qApp->removeTranslator(t);
//    } else {
        foreach( const QString & fileMask, m_Translators.keys() ) {
            QFileInfo f( fileMask );
            // this automatically send a QEvent::LanguageChange
            QString path = "";
            if ( fileMask.contains( QDir::separator() ) )
                path = f.absolutePath();
            else
                path = m_PathToTranslations;

            if ( !m_Translators[fileMask]->load( f.fileName() + "_" + lang, path ) )
                tkLog::addError( this, tr( "Can not load %1, path : %2" ).arg( f.fileName() + "_" + lang , path ) );
            else
                tkLog::addMessage( this, tkTr(FILE_1_LOADED).arg( f.fileName() + "_" + lang) );
        }
//    }
    emit languageChanged();
}

/**
 * \brief Add a translator to the known application translators. It can be for app or for plugins.
 * This member will automatically add the translator located at fileMask into QApplication translators.
 * @param fileMask : full path to qm file. Like this: "/path/to/qm/file" without "_en.qm" for example.
 * @return  true if all gone ok, false in the other case
 */
bool tkTranslators::addNewTranslator( const QString & fileMask, bool fromDefaultPath )
{
    // if necessary load toolkit translator
    if ( fileMask != "toolkit" )  // take care to infinite loop
        if ( m_Translators.isEmpty() )
            addNewTranslator( "toolkit" );

    QTranslator *t = new QTranslator( qApp );
    QString lang = QLocale().name().left( 2 );
    QString path;
    QFileInfo file( fileMask );

    // manage path
    if ( fromDefaultPath )
        path = m_PathToTranslations;
    else
        path = file.absolutePath();

    // if translator loads
    if ( t->load( file.fileName() + "_" + lang, path  ) ) {
        // add it to the map and the application
        if ( !m_Translators.contains( QDir::cleanPath( fileMask ) ) ) {
            m_Translators.insert( QDir::cleanPath( fileMask ) , t );
            qApp->installTranslator( t );
            tkLog::addMessage( this, tr( "Add Translator %1." ).arg( file.fileName() + "_" + lang ) );
            return true;
        }
    }
    else
        tkLog::addMessage( this, tr( "WARNING : Can not be loaded %1 or already loaded." ).arg( file.absoluteFilePath() + "_" + lang )  );

    // something gone wrong so clean and exit the member
    delete t;
    return false;
}

/** \brief Adds a new translator from the \e path and with the template filename \e fileTemplate */
bool tkTranslators::addNewTranslator( const QString & path, const QString & fileTemplate )
{
    return addNewTranslator( path + QDir::separator() + fileTemplate, false );
}

/** \brief Returns the availables translated locales for the application. */
QStringList tkTranslators::availableLocales()
{
    return availableLocalesAndLanguages().keys();
}

/** \brief Returns the available translated languages for the application. It is based on the Qt qm files. */
QStringList tkTranslators::availableLanguages()
{
    return availableLocalesAndLanguages().values();
}

/** \brief Returns the available translated languages for the application. It is based on the Qt qm files. */
QMap<QString, QString> tkTranslators::availableLocalesAndLanguages()
{
    QMap<QString, QString> toReturn;
    toReturn.insert( "en", "English" );

    if ( m_PathToTranslations.isEmpty() )
        return toReturn;

    QDir dir( m_PathToTranslations );
    QStringList fileNames = dir.entryList( QStringList( "toolkit_*.qm" ) );
    foreach( QString s, fileNames ) {
        QString locale = s;
        locale.remove( 0, locale.indexOf( '_' ) + 1 );
        locale.truncate( locale.lastIndexOf( '.' ) );
        QTranslator translator;
        translator.load( s, m_PathToTranslations );
        QString lang = translator.translate( "tkTranslators", "English" );
        toReturn.insert( locale, lang );
    }
    return toReturn;
}
