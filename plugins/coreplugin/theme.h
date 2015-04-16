/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef THEMEPRIVATE_H
#define THEMEPRIVATE_H

#include <coreplugin/itheme.h>

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QIcon>
#include <QPointer>
#include <QCache>

/**
 * \file theme.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.10
 * \date 09 Aug 2009
*/

namespace Core {
namespace Internal {

class CORE_EXPORT ThemePrivate : public Core::ITheme
{
    Q_OBJECT
public:
    ThemePrivate( QObject *parent = 0, const int cacheSize = 100 );
    ~ThemePrivate();

    void setThemeRootPath( const QString & absPath );
    void setThemeRelativeRootPath( const QString & relativePathFromAppBinary );

    void refreshCache();
    void setCacheMaxCost( const int max );

    QIcon icon( const QString & fileName, IconSize size = SmallIcon );
    QString iconFullPath( const QString &fileName, IconSize size = SmallIcon );
    QPixmap splashScreenPixmap( const QString &fileName, const IconSize size = MediumIcon );
    QPixmap defaultGenderPixmap(int gender, const IconSize size = BigIcon);

    void createSplashScreen(const QString &fileName);
    void messageSplashScreen(const QString &msg);
    void finishSplashScreen(QWidget *widget);
    virtual QSplashScreen *splashScreen() const {return m_Splash;}

protected:
    void setSmallIconPath( const QString &absPath );
    void setMediumIconPath( const QString &absPath );
    void setBigIconPath( const QString &absPath );

private:
    QCache<QString, QIcon> m_IconCache;
    QString m_AbsolutePath;
    QString m_SmallIconPath, m_MediumIconPath, m_BigIconPath;
    QSplashScreen *m_Splash;
};

} // end Internal
} // end Core

#endif // THEMEPRIVATE_H
