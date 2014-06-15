/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "templatescore.h"
#include "templatebase.h"
#include "templatesview_p.h"

using namespace Templates;
using namespace Internal;

namespace Templates {
namespace Internal {
class TemplatesCorePrivate
{
public:
    TemplatesCorePrivate(TemplatesCore */*parent*/) :
        m_Base(0),
        m_Manager(0) //, q(parent)
    {
    }

public:
    TemplateBase *m_Base;
    TemplatesViewManager *m_Manager;

private:
//    TemplatesCore *q;
};
}  // Internal
}  // Templates

TemplatesCore *TemplatesCore::m_Instance = 0;

TemplatesCore &TemplatesCore::instance()
{
    return *m_Instance;
}

TemplatesCore::TemplatesCore(QObject *parent) :
    QObject(parent),
    d(new TemplatesCorePrivate(this))
{
    setObjectName("TemplatesCore");
    d->m_Base = new TemplateBase(this);
    m_Instance = this;
}

void TemplatesCore::init()
{
    d->m_Base->initialize();
    d->m_Manager = new TemplatesViewManager(this);
}

Internal::TemplateBase *TemplatesCore::templateBase() const
{
    return d->m_Base;
}

Internal::TemplatesViewManager *TemplatesCore::templateViewManager() const
{
    return d->m_Manager;
}
