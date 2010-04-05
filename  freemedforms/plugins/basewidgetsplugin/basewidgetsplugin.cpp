/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "basewidgetsplugin.h"
#include "baseformwidgets.h"
#include "baseformwidgetsoptionspage.h"

#include <coreplugin/dialogs/pluginaboutpage.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace BaseWidgets;

BaseWidgetsPlugin::BaseWidgetsPlugin() :
        m_Factory(0),
        m_OptionsPage(0)
{
    qWarning() << "creating BaseWidgetsPlugin";
}

BaseWidgetsPlugin::~BaseWidgetsPlugin()
{
    if (m_Factory) {
        removeObject(m_Factory);
        delete m_Factory;
        m_Factory = 0;
    }
    if (m_OptionsPage) {
//        removeObject(m_OptionsPage);
        delete m_OptionsPage;
        m_OptionsPage = 0;
    }
}

bool BaseWidgetsPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    qWarning() << "BaseWidgetsPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    m_Factory = new BaseWidgetsFactory(this);
    m_OptionsPage = new Internal::BaseFormWidgetsOptionsPage(this);
    m_Factory->initialize(arguments,errorString);
    return true;
}

void BaseWidgetsPlugin::extensionsInitialized()
{
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    addObject(m_Factory);
//    addObject(m_OptionsPage);
}


Q_EXPORT_PLUGIN(BaseWidgetsPlugin)
