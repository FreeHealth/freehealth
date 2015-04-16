/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2015 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "datapackmode.h"
#include "constants.h"
#include "datapackmodewidget.h"
#include "ui_datapackmodewidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/theme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/fdm_constants.h>

#include <translationutils/constants.h>
#include <datapackutils/servercreation/servercreationwidget.h>

using namespace DataPackPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

DataPackMode::DataPackMode(QObject *parent) :
    Core::IMode(parent),
    _widget(0)
{
    setDisplayName("DataPacks");
    setIcon(theme()->icon(Core::Constants::ICON_PACKAGE));
    setPriority(Core::Constants::FifthModeGroup + Core::Constants::FirstModeSubGroup);
    setId(Constants::MODE_DATAPACK_SERVER);
    setPatientBarVisibility(false);

    // create the mode widget
    _widget = new DataPack::ServerCreationWidget;

    // Populate UI with default path
    _widget->setDefaultScreeningPath(settings()->value(Core::Constants::S_DATAPACK_SERVER_OUTPUT_PATH).toString());
    _widget->setDefaultServerOutputPath(settings()->value(Core::Constants::S_FILEOUTPUT_PATH).toString());
    _widget->setDefaultPathForServerDescriptionFiles(QString("%1/%2")
                                                     .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                                                     .arg("global_resources/datapack_description"));
    setWidget(_widget);
}

DataPackMode::~DataPackMode()
{
}

void DataPackMode::postCoreInitialization()
{
}

#ifdef WITH_TESTS

void DataPackMode::test_runWidgetTests()
{
    // _widget->test_runAllTests();
}

#endif
