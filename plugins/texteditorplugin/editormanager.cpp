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
 *   This code is inspired of the Qt example : Text Edit                   *
 *   Adaptations to FreeMedForms and improvments by : Eric Maeker, MD      *
 *   eric.maeker@gmail.com                                                   *
 ***************************************************************************/

/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "editormanager.h"
#include "editoractionhandler.h"
#include "texteditor.h"

#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/contextmanager/icontext.h>
#include <coreplugin/icore.h>

#include <QApplication>

using namespace Editor;
using namespace Editor::Internal;

EditorManager *EditorManager::m_Instance = 0;
EditorManager *EditorManager::instance(QObject *parent)
{
    if (!m_Instance) {
        if (!parent)
            m_Instance = new EditorManager(qApp);
        else
            m_Instance = new EditorManager(parent);
    }
    return m_Instance;
}

EditorManager::EditorManager(QObject *parent) :
        EditorActionHandler(parent)
{
    if (!parent)
        this->setParent(qApp);
    setObjectName("TextEditorManager");
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*,Core::Context)),
            this, SLOT(updateContext(Core::IContext*,Core::Context)));
}

EditorManager::~EditorManager()
{
}

void EditorManager::updateContext(Core::IContext *object, const Core::Context &additionalContexts)
{
    Q_UNUSED(additionalContexts);
    TextEditor *editor = 0;
    do {
        if (!object) {
            if (!m_CurrentEditor)
                return;

            m_CurrentEditor = 0;
            break;
        }
        editor = qobject_cast<TextEditor *>(object->widget());
        if (!editor) {
            if (!m_CurrentEditor)
                return;

            m_CurrentEditor = 0;
            break;
        }

        if (editor == m_CurrentEditor) {
            return;
        }

    } while (false);
    if (editor) {
        setCurrentEditor(editor);
    }
}

