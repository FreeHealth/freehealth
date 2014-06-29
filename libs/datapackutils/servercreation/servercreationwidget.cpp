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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DataPack::ServerCreationWidget
 * Preconfigured widget for the datapack server creation using
 * XML datapack queue files. \n
 * User can add screening path, select packs to add to the server,
 * set the server output path and create it.
 */

#include "servercreationwidget.h"
#include "packcreationmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/pathchooser.h>
#include <translationutils/constants.h>

#include <QProgressDialog>
#include <QPointer>
#include <QVector>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>

#include <QDebug>

#include "ui_servercreationwidget.h"

using namespace DataPack;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace DataPack {
namespace Internal {
class ServerCreationWidgetPrivate
{
public:
    ServerCreationWidgetPrivate(ServerCreationWidget *parent) :
        ui(0),
        _packCreationModel(0),
        aGroupByServer(0),
        aGroupByQueue(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~ServerCreationWidgetPrivate()
    {
        delete ui;
    }

    void setupUi()
    {
        ui = new Ui::ServerCreationWidget();
        ui->setupUi(q);
        ui->serverPath->setPromptDialogTitle(ui->selectServerPathLabel->text());
    }

    void createModel()
    {
        if (!_packCreationModel)
            _packCreationModel = new PackCreationModel(q);
        _packCreationModel->setFormat(PackCreationModel::ShowByServer);
        ui->packView->setModel(_packCreationModel);
    }

    void createActions()
    {
        aGroupByServer = new QAction(q);
        aGroupByQueue = new QAction(q);
        ui->groupByButton->addAction(aGroupByServer);
        ui->groupByButton->addAction(aGroupByQueue);
        ui->groupByButton->setDefaultAction(aGroupByServer);
    }

public:
    Ui::ServerCreationWidget *ui;
    PackCreationModel *_packCreationModel;
    QAction *aGroupByServer, *aGroupByQueue;

private:
    ServerCreationWidget *q;
};
}  // namespace Internal
}  // namespace DataPack

/*! Constructor of the DataPackPlugin::Internal::ServerCreationWidget class */
ServerCreationWidget::ServerCreationWidget(QWidget *parent) :
    QWidget(parent),
    d(new ServerCreationWidgetPrivate(this))
{
    d->setupUi();
    d->createModel();
    d->createActions();

    connect(d->ui->screePathButton, SIGNAL(clicked()), this, SLOT(onAddScreeningPathButtonClicked()));
    retranslate();
}

//TODO: add a ListView dependencies to this plugin

/*! Destructor of the DataPackPlugin::Internal::ServerCreationWidget class */
ServerCreationWidget::~ServerCreationWidget()
{
    if (d)
        delete d;
    d = 0;
}

/**
 * If \e absPath exists, add the \e absPath to the screened path.
 * All queues found inside this path (recurcively) will be added to
 * the internal pack creation queue model.
 */
bool ServerCreationWidget::addScreeningPath(const QString &absPath)
{
    return d->_packCreationModel->addScreeningPath(absPath);
}

/**
 * Returns the number of checked packs in the tree view
 */
int ServerCreationWidget::numberOfCheckedPacks() const
{
    return d->_packCreationModel->getCheckedPacks().count();
}

/**
 * Slot, create the server according to the elements of the UI.
 * Returns \e true on success.
 */
bool ServerCreationWidget::onCreateServerRequested()
{
    return false;
}

bool ServerCreationWidget::onAddScreeningPathButtonClicked()
{
    if (d->ui->screeningPath->isValid()) {
        return addScreeningPath(d->ui->screeningPath->path());
    }
    return false;
}

void ServerCreationWidget::retranslate()
{
    d->aGroupByServer->setText(tr("Group by server"));
    d->aGroupByQueue->setText(tr("Group by queue"));
}

void ServerCreationWidget::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        d->ui->retranslateUi(this);
        retranslate();
    }
}
