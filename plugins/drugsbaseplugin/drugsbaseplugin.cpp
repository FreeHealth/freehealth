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
#include "drugsbaseplugin.h"
#include "drugsdatabaseaboutpage.h"
#include "drugstemplateprinter.h"
#include "drugsbase.h"
#include "drugsdatabaseselector.h"

#include <utils/log.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>

using namespace DrugsDB;

DrugsBasePlugin::DrugsBasePlugin()
{
    if (Utils::Log::warnPluginsCreation()) {
#ifdef FREEDIAMS
        qWarning() << "creating FREEDIAMS::DrugsBasePlugin";
#else
        qWarning() << "creating FREEMEDFORMS::DrugsBasePlugin";
#endif
    }
}

DrugsBasePlugin::~DrugsBasePlugin()
{
}

bool DrugsBasePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsBasePlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

//    DrugsDatabaseSelector::instance();
//    DrugsDatabaseSelector::instance()->getAllDatabaseInformations();

    // initialize DrugsBase
    Internal::DrugsBase::instance();

    return true;
}

void DrugsBasePlugin::extensionsInitialized()
{
    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("drugsbaseplugin");

    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsBasePlugin::extensionsInitialized";
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    addAutoReleasedObject(new DrugsDB::Internal::DrugsDatabaseAboutPage(this));
    addAutoReleasedObject(new DrugsDB::Internal::DrugsTemplatePrinter(this));
}


Q_EXPORT_PLUGIN(DrugsBasePlugin)
