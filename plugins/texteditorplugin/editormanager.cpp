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
/***************************************************************************
 *   This code is inspired of the Qt example : Text Edit                   *
 *   Adaptations to FreeMedForms and improvments by : Eric Maeker, MD      *
 *   eric.maeker@free.fr                                                   *
 ***************************************************************************/

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
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
    connect(Core::ICore::instance()->contextManager(),SIGNAL(contextChanged(Core::IContext*)),
            this, SLOT(updateContext(Core::IContext*)));
}

EditorManager::~EditorManager()
{
}

bool EditorManager::registerEditor(TextEditor *editor)
{
    return true;
}

void EditorManager::updateContext(Core::IContext *object)
{
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

