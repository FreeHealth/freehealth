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
 *   Main Developers:                                                      *
 *       %Author% <%AuthorEmail%>                             *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef %PluginName:u%_INTERNAL_%PluginName:u%CONTEXTUALWIDGET_H
#define %PluginName:u%_INTERNAL_%PluginName:u%CONTEXTUALWIDGET_H

#include <coreplugin/contextmanager/icontext.h>
#include <QWidget>
#include <QList>

/**
 * \file %PluginName:l%contextualwidget.h
 * \author %Author%
 * \version 0.8.0
 * \date
*/

namespace %PluginName% {
namespace Internal {
class %PluginName%Context;

class %PluginName%ContextualWidget : public QWidget
{
    Q_OBJECT

public:
    explicit %PluginName%ContextualWidget(QWidget *parent = 0);
    ~%PluginName%ContextualWidget();

    void addContexts(const QList<int> &contexts);

private:
    %PluginName%Context *m_Context;
};

class %PluginName%Context : public Core::IContext
{
    Q_OBJECT
public:
    %PluginName%Context(%PluginName%ContextualWidget *w) :
        Core::IContext(w), wgt(w)
    {
        setObjectName("%PluginName%Context");
    }

    void setContext(QList<int> c) { ctx = c; }
    void addContext(QList<int> c) { ctx.append(c); }
    QList<int> context() const { return ctx; }
    QWidget *widget() { return wgt; }

private:
    %PluginName%ContextualWidget *wgt;
    QList<int> ctx;
};

} // namespace Internal
} // namespace %PluginName%

#endif // %PluginName:u%_INTERNAL_%PluginName:u%CONTEXTUALWIDGET_H
