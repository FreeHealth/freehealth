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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "mainwindowpreferences.h"

#include <utils/log.h>
#include <utils/updatechecker.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/theme.h>
#include <coreplugin/constants_tokensandsettings.h>


// TODO: create a virtual user creator

using namespace MainWin::Internal;
using namespace Trans::ConstantTranslations;


static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }


VirtualPatientBasePage::VirtualPatientBasePage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{ setObjectName("VirtualPatientBasePage"); }

VirtualPatientBasePage::~VirtualPatientBasePage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString VirtualPatientBasePage::id() const { return objectName(); }
QString VirtualPatientBasePage::displayName() const { return tr("Virtual database"); }
QString VirtualPatientBasePage::category() const { return tkTr(Trans::Constants::GENERAL_PREFERENCES); }
QString VirtualPatientBasePage::title() const {return tr("Virtual database");}
int VirtualPatientBasePage::sortIndex() const
{
    return 0;
}

void VirtualPatientBasePage::finish() { delete m_Widget; }

QWidget *VirtualPatientBasePage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new VirtualDatabasePreferences(parent);
    return m_Widget;
}

