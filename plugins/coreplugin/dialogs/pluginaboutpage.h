/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef PLUGINABOUTPAGE_H
#define PLUGINABOUTPAGE_H

#include <coreplugin/core_exporter.h>
#include <extensionsystem/pluginspec.h>

#include <coreplugin/iaboutpage.h>
#include <QObject>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTextBrowser;
QT_END_NAMESPACE

namespace Core {

class CORE_EXPORT PluginAboutPage : public Core::IAboutPage
{
    Q_OBJECT
public:
    PluginAboutPage(ExtensionSystem::PluginSpec *spec, QObject *parent);
    ~PluginAboutPage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    int sortIndex() const    { return 10;}

    void refreshContents();

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<QWidget> m_Widget;
    ExtensionSystem::PluginSpec *m_Spec;
    QTreeWidget *tree;
    QTextBrowser *tb;
};

} // End Core

#endif // BASEFORMWIDGETABOUTPAGE_H
