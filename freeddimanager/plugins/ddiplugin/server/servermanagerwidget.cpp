/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
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
#include <ddiplugin/ddi/drugdruginteractiontablemodel.h>
#include <ddiplugin/ddicore.h>
#include <ddiplugin/database/ddidatabase.h>

#include <translationutils/constants.h>

#include <QDateTime>
#include <QPushButton>

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
        interactorNewModel(0),
        interactorUpModel(0),
        ddiNewModel(0),
        ddiUpModel(0),
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

    QString headerCssStyle()
    {
        return QString("style='font-weight: 600; "
                       "font-size: large; "
                       "text-decoration: underline; '"
                       "font-variant: small-caps");
    }
    // Create the proposal human HTML content
    QString htmlProposal()
    {
        QString html;
        QString tmp;
        for(int i=0; i < ddiNewModel->rowCount(); ++i) {
            tmp += ddiNewModel->humanReadableDrugDrugInteractionOverView(i);
            tmp += "<br>";
        }
        if (!tmp.isEmpty()) {
            html += QString("<p><h1 %1>%2</h1><br>%3</p>")
                    .arg(headerCssStyle())
                    .arg("New DDI")
                    .arg(tmp);
        }

        tmp.clear();
        for(int i=0; i < ddiUpModel->rowCount(); ++i) {
            tmp += ddiUpModel->humanReadableDrugDrugInteractionOverView(i);
            tmp += "<br>";
        }
        if (!tmp.isEmpty()) {
            html += QString("<p><h1 %1>%2</h1><br>%3</p>")
                    .arg(headerCssStyle())
                    .arg("Updated DDI")
                    .arg(tmp);
        }

        return html;
    }

public:
    Ui::ServerManagerWidget *ui;
    DrugInteractorFilteredTableModel *interactorNewModel, *interactorUpModel;
    DrugDrugInteractionFilteredTableModel *ddiNewModel, *ddiUpModel;

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

    QPushButton *send = d->ui->buttonBox->addButton(tr("Send to the FreeMedForms server"), QDialogButtonBox::ActionRole);
    QPushButton *print = d->ui->buttonBox->addButton(tr("Print your proposals"), QDialogButtonBox::ActionRole);
    QPushButton *help = d->ui->buttonBox->addButton(tr("Help"), QDialogButtonBox::HelpRole);

    // TODO: code here
    Q_UNUSED(send);
    Q_UNUSED(print);
    Q_UNUSED(help);
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
    d->interactorNewModel = new DrugInteractorFilteredTableModel(this);
    d->interactorNewModel->initialize();
    d->interactorNewModel->filterNewItems(d->lastServerUpdateDateTime().date());
    d->ui->interactorNew->setModel(d->interactorNewModel);

    d->interactorUpModel = new DrugInteractorFilteredTableModel(this);
    d->interactorUpModel->initialize();
    d->interactorUpModel->filterLastUpdated(d->lastServerUpdateDateTime().date());
    d->ui->interactorUpdates->setModel(d->interactorUpModel);

    d->ddiNewModel = new DrugDrugInteractionFilteredTableModel(this);
    d->ddiNewModel->initialize();
    d->ddiNewModel->filterNewItems(d->lastServerUpdateDateTime().date());
    d->ui->ddiNew->setModel(d->ddiNewModel);

    d->ddiUpModel = new DrugDrugInteractionFilteredTableModel(this);
    d->ddiUpModel->initialize();
    d->ddiUpModel->filterLastUpdated(d->lastServerUpdateDateTime().date());
    d->ui->ddiUpdates->setModel(d->ddiUpModel);

    d->ui->proposals->setHtml(d->htmlProposal());
    return true;
}

