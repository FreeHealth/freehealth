/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef CORETRANSLATORS_H
#define CORETRANSLATORS_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/icore.h>

#include <QObject>
#include <QMap>
#include <QLocale>

class QTranslator;

/**
 * \file translators.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 02 Apr 2011
*/

namespace Core {

class CORE_EXPORT Translators : public QObject
{
    Q_OBJECT
public:
    static Translators* instance( QObject * parent = 0) ;
    Translators(QObject * parent = 0);
    ~Translators();

    bool addNewTranslator( const QString & fileName, bool fromDefaultPath = true );
    bool addNewTranslator( const QString & path, const QString & file );

public Q_SLOTS:
    void changeLanguage(const QString &lang);
    void changeLanguage(QLocale::Language lang);

public:
    static bool setPathToTranslations( const QString & path );
    static QString pathToTranslations();

    static QStringList availableLocales();
    static QStringList availableLanguages();
    static QMap<QString, QString> availableLocalesAndLanguages();

Q_SIGNALS:
    void languageChanged();


private:
    QMap<QString, QTranslator*>   m_Translators;  // String is file location and mask
    static QString                m_PathToTranslations;
    static Translators           *m_Instance;
};

} // end Core

#endif  // CORETRANSLATORS_H
