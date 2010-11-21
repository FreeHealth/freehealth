/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "icdcentralwidget.h"
#include "icdmodel.h"
#include "icdcollectionmodel.h"
#include "icddialog.h"
#include "icdassociation.h"
#include "icdwidgetmanager.h"
#include "constants.h"

// TEST
#include "icdio.h"
// END TEST

#include <coreplugin/icore.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <utils/log.h>

#include "ui_icdcentralwidget.h"

#include <QToolBar>
#include <QDebug>

using namespace ICD;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}


namespace ICD {
namespace Internal {

class IcdCentralWidgetPrivate
{
public:
    IcdCentralWidgetPrivate(IcdCentralWidget *parent) :
            m_IcdSearchModel(0), m_CollectionModel(0), m_ToolBar(0), q(parent)
    {}

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
    #ifdef FREEMEDFORMS
                << Constants::A_TOGGLE_ICDSELECTOR
    #endif
                << Core::Constants::A_FILE_OPEN
                << Core::Constants::A_FILE_SAVE
                << Core::Constants::A_FILE_SAVEAS
                << Core::Constants::A_TEMPLATE_CREATE
                << Core::Constants::A_FILE_PRINTPREVIEW
                << Constants::A_PRINT_COLLECTION
                ;

        foreach(const QString &s, actionsToAdd) {
            cmd = actionManager()->command(s);
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
            cmd = actionManager()->command(s);
            if (cmd)
                m_ToolBar->addAction(cmd->action());
        }
        m_ToolBar->addSeparator();

        m_ToolBar->setFocusPolicy(Qt::ClickFocus);
    }

public:
    IcdSearchModel *m_IcdSearchModel;
    IcdCollectionModel *m_CollectionModel;
    QToolBar *m_ToolBar;

private:
    IcdCentralWidget *q;
};
}  // End namespace Internal
}  // End namespace ICD



IcdCentralWidget::IcdCentralWidget(QWidget *parent) :
    IcdContextualWidget(parent),
    ui(new Ui::IcdCentralWidget),
    d(new Internal::IcdCentralWidgetPrivate(this))
{
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
    ui->collectionLayout->insertWidget(0, d->m_ToolBar);
}

IcdCentralWidget::~IcdCentralWidget()
{
    delete ui;
    if (d)
        delete d;
    d = 0;
}

void IcdCentralWidget::onSelectorActivated(const QVariant &SID)
{
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
        ui->collectionView->hideColumn(ICD::IcdCollectionModel::CodeWithoutDaget);
        ui->collectionView->hideColumn(ICD::IcdCollectionModel::HumanReadableDaget);
        ui->collectionView->hideColumn(ICD::IcdCollectionModel::SID);
        ui->collectionView->hideColumn(ICD::IcdCollectionModel::DagCode);
        ui->collectionView->expandAll();

        // TEST
//        IcdIO io;
//        qWarning() << io.icdCollectionToXml(d->m_CollectionModel);
        // END TEST
    }
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
