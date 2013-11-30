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
 * \class DDI::Internal::ServerManagerWidget
 * \brief short description of class
 *
 * Long description of class
 * \sa DDI::
 */

#include "servermanagerwidget.h"
#include "ui_servermanagerwidget.h"

#include <ddiplugin/interactors/druginteractortablemodel.h>
#include <ddiplugin/ddicore.h>
#include <ddiplugin/database/ddidatabase.h>

#include <translationutils/constants.h>

#include <QDateTime>

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DDI::DDICore *ddiCore()  { return DDI::DDICore::instance(); }
static inline DDI::Internal::DDIDatabase &ddiBase()  { return DDI::DDICore::instance()->database(); }

namespace DDI {
namespace Internal {
class ServerManagerWidgetPrivate
{
public:
    ServerManagerWidgetPrivate(ServerManagerWidget *parent) :
        ui(new Ui::ServerManagerWidget),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~ServerManagerWidgetPrivate()
    {
        delete ui;
    }

    // Returns the last date of server update extracted from the database,
    // or, if the user never sent update to the server, the database date of release
    QDateTime lastServerUpdateDateTime()
    {
        // Get latest server update (last time the user sent his data to the FreeMedForms server)
        QDateTime lastUpdate = ddiBase().dateOfLastServerUpdate();
        if (!lastUpdate.isValid())
            lastUpdate = ddiBase().dateOfRelease();
        return lastUpdate;
    }

public:
    Ui::ServerManagerWidget *ui;

private:
    ServerManagerWidget *q;
};
} // namespace Internal
} // end namespace DDI


/*! Constructor of the DDI::Internal::ServerManagerWidget class */
ServerManagerWidget::ServerManagerWidget(QWidget *parent) :
    QWidget(parent),
    d(new ServerManagerWidgetPrivate(this))
{
    d->ui->setupUi(this);
}

/*! Destructor of the DDI::Internal::ServerManagerWidget class */
ServerManagerWidget::~ServerManagerWidget()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool ServerManagerWidget::initialize()
{
    DrugInteractorFilteredTableModel *model = new DrugInteractorFilteredTableModel(this);
    model->initialize();
    model->filterLastUpdated(d->lastServerUpdateDateTime().date());
    d->ui->interactorUpdateView->setModel(model);
    return true;
}

