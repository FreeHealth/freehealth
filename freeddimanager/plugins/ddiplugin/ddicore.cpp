/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DDI::DDICore
 * \brief Central core singleton object for the DDIManager plugin.
 */

#include "ddicore.h"
#include <ddiplugin/constants.h>
#include <ddiplugin/atc/atctablemodel.h>
#include <ddiplugin/interactors/druginteractortablemodel.h>
#include <ddiplugin/database/ddidatabase.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

DDI::DDICore *DDI::DDICore::_instance = 0;

namespace DDI {
namespace Internal {
class DDICorePrivate
{
public:
    DDICorePrivate(DDICore *parent) :
        _atcTableModel(0),
        _drugInteractorTableModel(0),
        _base(0),
        q(parent)

    {
        Q_UNUSED(q);
    }

    ~DDICorePrivate()
    {
        if (_base)
            delete _base;
        _base = 0;
    }

public:
    AtcTableModel *_atcTableModel;
    DrugInteractorTableModel *_drugInteractorTableModel;
    DDIDatabase *_base;

private:
    DDICore *q;
};
} // namespace Internal
} // namespace DDI

/*! Constructor of the DrugsDB::DDICore class */
DDICore::DDICore(QObject *parent) :
    QObject(parent),
    d(new DDICorePrivate(this))
{
    setObjectName("DDICore");
    _instance = this;
}

/** Return the single instance of the object */
DDICore *DDICore::instance()
{
    return _instance;
}

/*! Destructor of the DrugsDB::DDICore class */
DDICore::~DDICore()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool DDICore::initialize()
{
    d->_base = new DDIDatabase;
    d->_base->initialize(settings()->databasePath(), true);
//    d->_base->insertAtcDataFromCsv(settings()->path(Core::ISettings::BundleResourcesPath) + Constants::ATC_CSV_FILENAME);
//    d->_base->insertDrugInteractorsDataFromXml(settings()->path(Core::ISettings::BundleResourcesPath) + Constants::INTERACTORS_XML_FILENAME);

    d->_atcTableModel = new AtcTableModel(this);
    d->_atcTableModel->initialize();

    d->_drugInteractorTableModel = new DrugInteractorTableModel(this);
    d->_drugInteractorTableModel->initialize();
    return true;
}

/** Returns the Atc table model single instance */
AtcTableModel *DDICore::atcTableModel() const
{
    return d->_atcTableModel;
}

DrugInteractorTableModel *DDICore::drugInteractorTableModel() const
{
    return 0;
}

/** Returns the DDI database single instance */
Internal::DDIDatabase &DDICore::database() const
{
    return *d->_base;
}
