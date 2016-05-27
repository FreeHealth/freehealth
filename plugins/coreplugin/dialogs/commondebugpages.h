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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef COMMONDEBUGPAGE_H
#define COMMONDEBUGPAGE_H

#include <coreplugin/idebugpage.h>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

namespace Core {
namespace Internal {

class LogMessageDebugPage : public IDebugPage
{
    Q_OBJECT
public:
    LogMessageDebugPage(QObject *parent);
    ~LogMessageDebugPage();

    QString id() const       { return objectName();   }
    QString displayName() const;
    QString category() const;
    int sortIndex() const    { return 20;}

    QWidget *createPage(QWidget *parent = 0);
};


class LogErrorDebugPage : public IDebugPage
{
    Q_OBJECT
public:
    LogErrorDebugPage(QObject *parent);
    ~LogErrorDebugPage();

    QString id() const       { return objectName();   }
    QString displayName() const;
    QString category() const;
    int sortIndex() const    { return 10;}

    QWidget *createPage(QWidget *parent = 0);
};


class SettingDebugPage : public IDebugPage
{
    Q_OBJECT
public:
    SettingDebugPage(QObject *parent);
    ~SettingDebugPage();

    QString id() const       { return objectName();   }
    QString displayName() const     { return tr("Settings"); }
    QString category() const { return tr("Settings"); }
    int sortIndex() const    { return 30;}

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<QWidget> m_Widget;
    QTreeWidget *tree;
};

} // End Internal
} // End Core

#endif // COMMONDEBUGPAGE_H
