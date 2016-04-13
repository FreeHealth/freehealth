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
 ***************************************************************************/
#include "icdcentralwidget.h"
#include "icdsearchmodel.h"
#include "icdcollectionmodel.h"
#include "icddialog.h"
#include "icdassociation.h"
#include "icdwidgetmanager.h"
#include "constants.h"
#include "icddatabase.h"
#include "icdio.h"
#include "icdplugin.h"

#include <coreplugin/icore.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/idocumentprinter.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include "ui_icdcentralwidget.h"

#include <QToolBar>
#include <QLabel>

#include <QDebug>

using namespace ICD;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}
static inline ICD::IcdDatabase *icdBase() {return ICD::IcdDatabase::instance();}

namespace ICD {
namespace Internal {

class IcdCentralWidgetPrivate
{
public:
    IcdCentralWidgetPrivate(IcdCentralWidget *parent) :
            m_IcdSearchModel(0), m_CollectionModel(0), m_ToolBar(0), m_ModeLabel(0),
            q(parent)
    {
    }

    void createActionsAndToolBar()
    {
        Core::Command *cmd = 0;
        // populate toolbar
        m_ToolBar = new QToolBar(q);
    #if QT_VERSION < 0x040600
        m_ToolBar->setIconSize(QSize(16,16));
    #endif
        QStringList actionsToAdd;
        actionsToAdd
                << Constants::A_TOGGLE_ICDSELECTOR
                << Core::Constants::A_FILE_OPEN
                << Core::Constants::A_FILE_SAVE
                << Core::Constants::A_FILE_SAVEAS
                << Core::Constants::A_TEMPLATE_CREATE
                << Core::Constants::A_FILE_PRINTPREVIEW
                << Constants::A_PRINT_COLLECTION
                ;

        foreach(const QString &s, actionsToAdd) {
            cmd = actionManager()->command(Core::Id(s));
            if (cmd)
                m_ToolBar->addAction(cmd->action());
        }

        actionsToAdd.clear();
        actionsToAdd
                << Core::Constants::A_LIST_CLEAR
                << Core::Constants::A_LIST_REMOVE
                ;
        m_ToolBar->addSeparator();
        foreach(const QString &s, actionsToAdd) {
            cmd = actionManager()->command(Core::Id(s));
            if (cmd)
                m_ToolBar->addAction(cmd->action());
        }
        m_ToolBar->addSeparator();

        m_ModeLabel = new QLabel(q);
        QWidget *w = new QWidget(q);
        w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_ToolBar->addWidget(w);
        m_ToolBar->addSeparator();
        m_ToolBar->addWidget(m_ModeLabel);

        m_ToolBar->setFocusPolicy(Qt::ClickFocus);
    }

public:
    IcdSearchModel *m_IcdSearchModel;
    IcdCollectionModel *m_CollectionModel;
    QToolBar *m_ToolBar;
    int m_SelectorMode, m_CollectionMode;
    QLabel *m_ModeLabel;

private:
    IcdCentralWidget *q;
};
}  // End namespace Internal
}  // End namespace ICD

/** CTor */
IcdCentralWidget::IcdCentralWidget(QWidget *parent) :
    IcdContextualWidget(parent),
    ui(new Ui::IcdCentralWidget),
    d(new Internal::IcdCentralWidgetPrivate(this))
{
    d->m_SelectorMode = SelectorFullMode;
    d->m_CollectionMode = CollectionFullMode;

    // Ensure that manager is instanciated
    IcdWidgetManager::instance();

    // Views and models
    d->m_IcdSearchModel = new ICD::IcdSearchModel(this);
    ui->setupUi(this);
    ui->selector->setModel(d->m_IcdSearchModel);
    ui->selector->initialize();
    d->m_CollectionModel = new IcdCollectionModel(this);
    ui->collectionView->setModel(d->m_CollectionModel);
    ui->collectionView->header()->setStretchLastSection(true);
    ui->collectionView->header()->hide();
    connect(ui->selector, SIGNAL(activated(QVariant)), this, SLOT(onSelectorActivated(QVariant)));

    // ToolBar
    d->createActionsAndToolBar();
    d->m_ModeLabel->setText(tr("Collection"));
    ui->collectionLayout->insertWidget(0, d->m_ToolBar);
}

/** DTor */
IcdCentralWidget::~IcdCentralWidget()
{
    delete ui;
    if (d)
        delete d;
    d = 0;
}

/** Internal slot. Called when user dble-click on an ICD code */
void IcdCentralWidget::onSelectorActivated(const QVariant &SID)
{
    if (d->m_SelectorMode == SelectorSimpleMode) {
        d->m_CollectionModel->addCode(SID);
    } else {
        if (!d->m_CollectionModel->canAddThisCode(SID, false)) {
            Utils::informativeMessageBox(tr("Can not add this code to your collection."),
                                         tr("This code is already included or is excluded by the "
                                            "current collection code."));
            return;
        }
        ICD::IcdDialog dlg(SID, this);
        if (dlg.exec()==QDialog::Accepted) {
            if (!dlg.isSelectionValid())
                return;
            if (dlg.isUniqueCode())
                d->m_CollectionModel->addCode(dlg.getSidCode());
            else if (dlg.isAssociation()) {
                foreach(const Internal::IcdAssociation &asso, dlg.getAssocation())
                    d->m_CollectionModel->addAssociation(asso);
            }
            ui->collectionView->expandAll();
        }
    }
    ui->collectionView->hideColumn(ICD::IcdCollectionModel::CodeWithoutDaget);
    ui->collectionView->hideColumn(ICD::IcdCollectionModel::HumanReadableDaget);
    ui->collectionView->hideColumn(ICD::IcdCollectionModel::SID);
    ui->collectionView->hideColumn(ICD::IcdCollectionModel::DagCode);
}

void IcdCentralWidget::setSelectorMode(const SelectorModes mode)
{d->m_SelectorMode = mode;}

IcdCentralWidget::SelectorModes IcdCentralWidget::selectorMode() const
{return SelectorModes(d->m_SelectorMode);}

void IcdCentralWidget::setCollectionMode(const CollectionModes mode)
{
    d->m_CollectionMode = mode;
    if (d->m_CollectionModel) {
        if (mode==CollectionSimpleMode) {
            d->m_CollectionModel->setCollectionIsSimpleList(true);
        } else {
            d->m_CollectionModel->setCollectionIsSimpleList(false);
        }
    }
}

IcdCentralWidget::CollectionModes IcdCentralWidget::collectionMode() const
{return CollectionModes(d->m_CollectionMode);}

/**
  \brief Defines the ICD::IcdCollectionModel to use.
  By default, the IcdCentralWidget creates its own model, if you set yours, the current model will be deleted and set to zero
*/
void IcdCentralWidget::setIcdCollectionModel(IcdCollectionModel *model)
{
    if (d->m_CollectionModel) {
        delete d->m_CollectionModel;
        d->m_CollectionModel = 0;
    }
    d->m_CollectionModel = model;
    ui->collectionView->setModel(model);
}

IcdCollectionModel *IcdCentralWidget::icdCollectionModel() const
{
    return d->m_CollectionModel;
}

void IcdCentralWidget::openFile(const QString &file)
{
    if (d->m_CollectionModel->rowCount() > 0) {
        int r = Utils::withButtonsMessageBox(
                tr("Opening an ICD collection: merge or replace?"),
                tr("There is an ICD collection inside the editor, "
                   "do you to replace it or to add the opened collection?"),
                QString(), QStringList() << tr("Replace collection") << tr("Add to collection"),
                tr("Open a collection") + " - " + qApp->applicationName());
        if (r == 0) {
            IcdIO io;
            io.icdCollectionFromXml(d->m_CollectionModel, Utils::readTextFile(file, Utils::DontWarnUser));
        } else if (r==1) {
            IcdIO io;
            io.icdCollectionFromXml(d->m_CollectionModel, Utils::readTextFile(file, Utils::DontWarnUser), ICD::IcdIO::AddToModel);
        }
    } else {
        IcdIO io;
        io.icdCollectionFromXml(d->m_CollectionModel, Utils::readTextFile(file, Utils::DontWarnUser));
    }
    ui->collectionView->hideColumn(ICD::IcdCollectionModel::CodeWithoutDaget);
    ui->collectionView->hideColumn(ICD::IcdCollectionModel::HumanReadableDaget);
    ui->collectionView->hideColumn(ICD::IcdCollectionModel::SID);
    ui->collectionView->hideColumn(ICD::IcdCollectionModel::DagCode);
    ui->collectionView->expandAll();
}

void IcdCentralWidget::readXmlCollection(const QString &xml)
{
    IcdIO io;
    io.icdCollectionFromXml(d->m_CollectionModel, xml, IcdIO::ReplaceModelContent);
    ui->collectionView->hideColumn(ICD::IcdCollectionModel::CodeWithoutDaget);
    ui->collectionView->hideColumn(ICD::IcdCollectionModel::HumanReadableDaget);
    ui->collectionView->hideColumn(ICD::IcdCollectionModel::SID);
    ui->collectionView->hideColumn(ICD::IcdCollectionModel::DagCode);
}

QString IcdCentralWidget::collectionToXml() const
{
    IcdIO io;
    return io.icdCollectionToXml(d->m_CollectionModel);
}

void IcdCentralWidget::toggleSelector()
{
    ui->selector->setVisible(!ui->selector->isVisible());
}

void IcdCentralWidget::clear()
{
    if (d->m_CollectionModel) {
        d->m_CollectionModel->clearCollection();
    }
}

void IcdCentralWidget::removeItem()
{
    const QModelIndex &index = ui->collectionView->currentIndex();
    d->m_CollectionModel->removeRow(index.row(), index.parent());
}

void IcdCentralWidget::print()
{
    IcdIO io;
    QString toPrint = io.icdCollectionToHtml(d->m_CollectionModel);

    Core::IDocumentPrinter *p = printer();
    if (!p) {
        Utils::Log::addError(this, "No IDocumentPrinter found", __FILE__, __LINE__);
        return;
    }
    p->clearTokens();
    QHash<QString, QVariant> tokens;
    tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, QCoreApplication::translate(Constants::ICDCONSTANTS_TR_CONTEXT, Constants::ICD_CODECOLLECTION_TEXT));
    tokens.insert(Constants::T_IS_PRINTING_ICDCOLLECTION, " ");
    tokens.insert(Constants::T_ICD_DATABASE_VERSION, icdBase()->getDatabaseVersion());
//    tokens.insert(Constants::T_ICD_PLUGIN_VERSION, IcdPlugin::pluginVersion());
    p->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);
    p->print(toPrint, Core::IDocumentPrinter::Papers_Generic_User, false);
}

void IcdCentralWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
