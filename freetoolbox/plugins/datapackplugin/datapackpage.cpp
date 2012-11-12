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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DataPackPlugin::Internal::DataPackPage
 * \brief short description of class
 *
 * Long description of class
 * \sa DataPackPlugin::
 */

#include "datapackpage.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QTimer>

#include <QDebug>

using namespace DataPackPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

namespace DataPackPlugin {
namespace Internal {
class DataPackPagePrivate
{
public:
    DataPackPagePrivate(DataPackPage *parent) :
        q(parent)
    {
    }
    
    ~DataPackPagePrivate()
    {
    }
    
private:
    DataPackPage *q;
};
}  // namespace Internal
} // end namespace DataPackPlugin

/*! Constructor of the DataPackPlugin::Internal::DataPackPage class */
DataPackPage::DataPackPage(QObject *parent) :
    Core::IToolPage(parent),
    d(new DataPackPagePrivate(this))
{
}

/*! Destructor of the DataPackPlugin::Internal::DataPackPage class */
DataPackPage::~DataPackPage()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool DataPackPage::initialize()
{
    return true;
}

QString DataPackPage::name() const
{
    return tkTr(Trans::Constants::DATAPACK);
}

QString DataPackPage::category() const
{
    return tkTr(Trans::Constants::DATAPACK);
}

QIcon DataPackPage::icon() const
{
    return theme()->icon(Core::Constants::ICON_PACKAGE);
}

QWidget *DataPackPage::createPage(QWidget *parent)
{
    return new QWidget(parent);
}

DataPackStep::DataPackStep(QObject *parent) :
    Core::IFullReleaseStep(parent)
{}

DataPackStep::~DataPackStep()
{}

bool DataPackStep::downloadFiles(QProgressBar *bar)
{
    Q_UNUSED(bar);
    QTimer::singleShot(10, this, SIGNAL(downloadFinished()));
    return true;
}

bool DataPackStep::process()
{
    // At this point all IFullReleaseStep must have register their datapacks to the datapackcore
    return true;
}

QString DataPackStep::processMessage() const
{
    return tr("Processing datapack servers");
}

