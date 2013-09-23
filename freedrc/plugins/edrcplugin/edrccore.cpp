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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class eDRC::EdrcCore
 * \brief short description of class
 *
 * Long description of class
 * \sa eDRC::
 */

#include "edrccore.h"
#include <edrcplugin/database/constants_db.h>
#include <edrcplugin/database/edrcbase.h>
#include <edrcplugin/consultresult.h>
#include <edrcplugin/constants.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <translationutils/constants.h>

#include <QFileInfo>
#include <QDir>

#include <QDebug>

using namespace eDRC;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

namespace eDRC {
namespace Internal {
class EdrcCorePrivate
{
public:
    EdrcCorePrivate(EdrcCore *parent) :
        _edrcBase(0),
        q(parent)
    {
    }
    
    ~EdrcCorePrivate()
    {
    }
    
    // Find the database to use. In priority order:
    // - User datapack
    // - Application installed datapack
    QString databasePath() const
    {
        QString dbRelPath = QString("/%1/%2").arg(Constants::DB_NAME).arg(Constants::DB_FILENAME);
        QString tmp;
        tmp = settings()->dataPackInstallPath() + dbRelPath;
        if (QFileInfo(tmp).exists())
            return settings()->dataPackInstallPath();
        return settings()->dataPackApplicationInstalledPath();
    }

    // Returns the path to the CSV raw source for the eDRC database
    QString csvFilesPath() const
    {
        QString pathToCsv = settings()->path(Core::ISettings::BundleResourcesPath);
        pathToCsv += "/nonfree/edrc";
        return pathToCsv;
    }

public:
    DrcDatabase *_edrcBase;
    
private:
    EdrcCore *q;
};
} // namespace Internal
} // end namespace eDRC

// Singleton initialization
EdrcCore *EdrcCore::_instance = 0;

/**
 * Singleton access. This object creates its instance in the Ctor. So you should never
 * request the ctor more than once.
 */
eDRC::EdrcCore &eDRC::EdrcCore::instance() // static
{
    Q_ASSERT(_instance);
    return *_instance;
}

/*! Constructor of the eDRC::EdrcCore class */
EdrcCore::EdrcCore(QObject *parent) :
    QObject(parent),
    d(new EdrcCorePrivate(this))
{
    _instance = this;
}

/*! Destructor of the eDRC::EdrcCore class */
EdrcCore::~EdrcCore()
{
    _instance = 0;
    delete d->_edrcBase;
    d->_edrcBase = 0;
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool EdrcCore::initialize()
{
    d->_edrcBase = new DrcDatabase(d->databasePath());
    d->_edrcBase->initialize(true, d->csvFilesPath());
    return true;
}

/**
 * Transform a eDRC::Internal::ConsultResult to HTML using the current
 * database and the current settings.
 */
QString EdrcCore::toHtml(const Internal::ConsultResult &cr)
{
    return cr.toHtml(settings()->value(Constants::S_TOKEN_HTMLGLOBALMASK).toString(),
                     settings()->value(Constants::S_TOKEN_HTMLCRITERIASMASK).toString(),
                     *d->_edrcBase);
}

DrcDatabase &EdrcCore::edrcBase() const
{
    return *d->_edrcBase;
}
