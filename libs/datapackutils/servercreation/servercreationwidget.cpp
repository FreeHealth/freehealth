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
 *
 * You can set the default screening path with setDefaultScreeningPath().
 * For a full automated server creation, set the path to the server
 * description files (which is by default {ApplicationResources/datapack_description})
 * using setDefaultPathForServerDescriptionFiles()
 */

#include "servercreationwidget.h"
#include "packcreationmodel.h"
#include "packservercreator.h"

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
        ui->serverPath->setExpectedKind(Utils::PathChooser::Directory);
        ui->screeningPath->setExpectedKind(Utils::PathChooser::Directory);
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

    connect(d->ui->screenPathButton, SIGNAL(clicked()), this, SLOT(onAddScreeningPathButtonClicked()));
    connect(d->_packCreationModel, SIGNAL(layoutChanged()), this, SLOT(updateTotalNumberOfPacks()));
    connect(d->ui->createServer, SIGNAL(clicked()), this, SLOT(onCreateServerRequested()));
    retranslate();
}

/*! Destructor of the DataPackPlugin::Internal::ServerCreationWidget class */
ServerCreationWidget::~ServerCreationWidget()
{
    if (d)
        delete d;
    d = 0;
}

/**
 * Set the line edit "Screen path" default content
 */
void ServerCreationWidget::setDefaultScreeningPath(const QString &absPath)
{
    d->ui->screeningPath->setInitialBrowsePathBackup(absPath);
    d->ui->screeningPath->setPath(absPath);
}

/**
 * Set the path to the server description files
 * (which is by default {ApplicationResources/datapack_description})
 * using setDefaultPathForServerDescriptionFiles()
 * \sa PackServerCreator::setServerDescriptionFilePath()
 */
bool ServerCreationWidget::setDefaultPathForServerDescriptionFiles(const QString &absPath)
{
    d->ui->serverDescrPath->setPath(QString::null);
    if (QDir(absPath).exists()) {
        d->ui->serverDescrPath->setPath(absPath);
        return true;
    }
    return false;
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
 * Set the line edit "Server output" default content
 */
void ServerCreationWidget::setDefaultServerOutputPath(const QString &absPath)
{
    d->ui->serverPath->setInitialBrowsePathBackup(absPath);
    d->ui->serverPath->setPath(absPath);
}

/**
 * Slot, create the server according to the elements of the UI.
 * Returns \e true on success.
 */
bool ServerCreationWidget::onCreateServerRequested()
{
    QProgressDialog *prog = new QProgressDialog(this);
    QString title = tr("Creating Datapack Server\n%1").arg(d->ui->serverPath->path());
    prog->setLabelText(title);
    prog->setRange(0, 10);
    prog->show();
    title += "\n\n";
    int i = 0;

    prog->setLabelText(QString("%1* Checking data").arg(title));
    prog->setValue(i);
    // Path to server description files
    if (d->ui->serverDescrPath->path().isEmpty()) {
        delete prog;
        Utils::warningMessageBox(tr("Wrong server description file path"),
                                 tr("Please set a valid server description path."));
        return false;
    }
    // No output path -> error
    if (!d->ui->serverPath->isValid()) {
        delete prog;
        Utils::warningMessageBox(tr("Wrong server output path"),
                                 tr("Please set a valid server output path. \n"
                                    "The path must be created with read/write "
                                    "rights on."));
        return false;
    }

    // No Packs -> error
    if (numberOfCheckedPacks() == 0) {
        delete prog;
        Utils::warningMessageBox(tr("No Pack selected"),
                                 tr("No Pack selected. Please select all "
                                    "Packs you want to include in the server."));
        return false;
    }

    // Get the Queue for the server creation
    prog->setLabelText(QString("%1* Generating queue").arg(title));
    prog->setValue(++i);
    PackCreationQueue queue = d->_packCreationModel->generateQueueForServerCreation();
    PackServerCreator serverCreator;
    serverCreator.useDefaultPathForServerDescriptionFiles(d->ui->serverDescrPath->path());
    if (!serverCreator.addPackCreationQueue(queue)) {
        delete prog;
        LOG_ERROR("Unable to add the queue");
        return false;
    }

    prog->setLabelText(QString("%1* Creating server").arg(title));
    prog->setValue(++i);
    if (!serverCreator.createServer(d->ui->serverPath->path())) {
        delete prog;
        Utils::warningMessageBox(tr("Error"),
                                 tr("An error occured when trying to create the server."));
        return false;
    }

    delete prog;

    Utils::informativeMessageBox(tr("Server created"),
                             tr("The datapack server was correctly "
                                "created within the following path:\n"
                                "%1").arg(d->ui->serverPath->path()));
    return true;
}

/** When user click on "Screen Path", adds the path to the model */
bool ServerCreationWidget::onAddScreeningPathButtonClicked()
{
    if (d->ui->screeningPath->isValid()) {
        if (addScreeningPath(d->ui->screeningPath->path())) {
            updateTotalNumberOfPacks();
            return true;
        }
    }
    return false;
}

/** Update the UI with the total number of available Packs found by the model */
void ServerCreationWidget::updateTotalNumberOfPacks()
{
    d->ui->numberOfQueues->setText(QString::number(d->_packCreationModel->totalNumberOfPacksFound()));
}

/** Re translate the UI */
void ServerCreationWidget::retranslate()
{
    d->aGroupByServer->setText(tr("Group by server"));
    d->aGroupByQueue->setText(tr("Group by queue"));
    d->ui->serverPath->setPromptDialogTitle(d->ui->selectServerPathLabel->text());
    d->ui->screeningPath->setPromptDialogTitle(d->ui->labelScreenPath->text());
}

void ServerCreationWidget::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        d->ui->retranslateUi(this);
        retranslate();
    }
}
