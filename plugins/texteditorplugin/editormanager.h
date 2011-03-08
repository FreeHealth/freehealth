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
#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include <texteditorplugin/texteditor_exporter.h>
#include <texteditorplugin/editoractionhandler.h>

#include <QObject>

namespace Core {
class IContext;
}

namespace Editor {
class TextEditor;

namespace Internal {

class EditorManager : public EditorActionHandler
{
    Q_OBJECT
public:
    static EditorManager *instance(QObject *parent = 0);
    virtual ~EditorManager();

private Q_SLOTS:
    void updateContext(Core::IContext *context );

private:
    EditorManager(QObject *parent = 0);
    static EditorManager *m_Instance;
};

}  // End Internal
}  // End Editor

#endif // EDITORMANAGER_H
