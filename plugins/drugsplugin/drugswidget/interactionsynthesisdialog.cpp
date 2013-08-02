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
#include "interactionsynthesisdialog.h"

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/interactionmanager.h>
#include <drugsbaseplugin/idruginteraction.h>
#include <drugsbaseplugin/druginteractionresult.h>
#include <drugsbaseplugin/druginteractionquery.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/idocumentprinter.h>

#include <utils/log.h>
#include <medicalutils/ebmdata.h>
#include <medicalutils/ebmmodel.h>
#include <translationutils/constanttranslations.h>

#include <extensionsystem/pluginmanager.h>

#include "ui_interactionsynthesisdialog.h"

#include <QToolButton>
#include <QToolBar>
#include <QMultiHash>
#include <QStandardItemModel>

using namespace DrugsWidget;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}

namespace DrugsWidget {
namespace Internal {
class InteractionSynthesisDialogPrivate
{
public:
    InteractionSynthesisDialogPrivate() :
            ui(0), m_DrugModel(0), m_InteractionModel(0),
            m_InteractionQueryModel(0), m_InteractionResult(0)
    {}

    ~InteractionSynthesisDialogPrivate()
    {
        delete ui;
        qDeleteAll(m_Biblio.values());
        m_Biblio.clear();
    }

    void populateDrugsBrowser(DrugsDB::IDrugInteraction *interaction, QTextBrowser *browser)
    {
        browser->clear();
        ui->interactionQueryView->selectionModel()->clear();
        QString drugs;
        foreach(DrugsDB::IDrug *drug, interaction->drugs()) {
            for(int i=0; i<m_InteractionQueryModel->rowCount();++i) {
                if (m_InteractionQueryModel->index(i, 0).data(Qt::UserRole+1)==drug->drugId()) {
                    ui->interactionQueryView->selectionModel()->select(m_InteractionQueryModel->index(i, 0), QItemSelectionModel::Select);
                }
            }
        }
        browser->setHtml(interaction->toHtml());
    }

public:
    Ui::InteractionSynthesisDialog *ui;
    DrugsDB::DrugsModel *m_DrugModel;
    QAction *aPrintAll, *aPrintOne;
    QMultiHash<DrugsDB::IDrugInteraction *, MedicalUtils::EbmData *> m_Biblio;
    QStandardItemModel *m_InteractionModel, *m_InteractionQueryModel;
    DrugsDB::DrugInteractionResult *m_InteractionResult;
    QToolButton *close, *report, *print, *help;
    MedicalUtils::EbmModel *m_TreeEbmModel, *m_DdiEbmModel;
};
}
}

InteractionSynthesisDialog::InteractionSynthesisDialog(DrugsDB::DrugsModel *drugModel, QWidget *parent) :
    QDialog(parent),
    d(new Internal::InteractionSynthesisDialogPrivate)
{
    Q_ASSERT(drugModel);
    if (!drugModel)
        return;
    d->ui = new Ui::InteractionSynthesisDialog;

    d->m_DrugModel = drugModel;

    // create and populate the views
    d->ui->setupUi(this);
    setWindowTitle(tr("Synthetic interactions") + " - " + qApp->applicationName());
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);

//    QToolBar *bar = new QToolBar(this);
//    bar->setIconSize(QSize(32,32));
//    bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
//    d->ui->toolbarLayout->addWidget(bar);

    d->aPrintAll = new QAction(this);
    d->aPrintAll->setText(tr("Print all interactions"));
    d->aPrintAll->setShortcut(QKeySequence::Print);
    d->aPrintAll->setIcon(theme()->icon(Core::Constants::ICONPRINT, Core::ITheme::MediumIcon));
    d->aPrintAll->setData(-1);
//    bar->addAction(d->aPrintAll);

    d->aPrintOne = new QAction(this);
    d->aPrintOne->setText(tr("Print selected interaction"));
    d->aPrintOne->setShortcut(QKeySequence::Print);
    d->aPrintOne->setIcon(theme()->icon(Core::Constants::ICONPRINT, Core::ITheme::MediumIcon));
    d->aPrintOne->setData(-1);
//    bar->addAction(d->aPrintOne);

//    connect(bar, SIGNAL(actionTriggered(QAction*)), this, SLOT(levelActivated(QAction*)));
//    connect(d->aPrintAll, SIGNAL(triggered()), this, SLOT(print()));
//    connect(d->aPrintOne, SIGNAL(triggered()), this, SLOT(print()));

    // populate the button box
    d->close = new QToolButton(this);
    d->close->setIconSize(QSize(32, 32));
    d->close->setText(tkTr(Trans::Constants::CLOSE_TEXT));
    d->close->setIcon(theme()->icon(Core::Constants::ICONQUIT, Core::ITheme::MediumIcon));
    d->close->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    d->print = new QToolButton(this);
    d->print->setIconSize(QSize(32, 32));
    d->print->addAction(d->aPrintAll);
    d->print->addAction(d->aPrintOne);
    d->print->setDefaultAction(d->aPrintAll);
    d->print->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(d->print, SIGNAL(triggered(QAction*)), this, SLOT(print(QAction*)));

    d->report = new QToolButton(this);
    d->report->setIconSize(QSize(32, 32));
    d->report->setText(tr("Send a report"));
    d->report->setIcon(theme()->icon(DrugsDB::Constants::I_DRUGREPORT, Core::ITheme::MediumIcon));
    d->report->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(d->report, SIGNAL(clicked()), this, SLOT(drugReportRequested()));

    d->help = new QToolButton(this);
    d->help->setIconSize(QSize(32, 32));
    d->help->setText(tkTr(Trans::Constants::HELP_TEXT));
    d->help->setIcon(theme()->icon(Core::Constants::ICONHELP, Core::ITheme::MediumIcon));
    d->help->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    d->ui->buttonBox->addButton(d->close, QDialogButtonBox::YesRole);
    d->ui->buttonBox->addButton(d->report, QDialogButtonBox::ActionRole);
    d->ui->buttonBox->addButton(d->print, QDialogButtonBox::ActionRole);
    d->ui->buttonBox->addButton(d->help, QDialogButtonBox::HelpRole);

    d->m_InteractionResult = drugModel->drugInteractionResult();
    d->m_InteractionModel = drugModel->drugInteractionResult()->toStandardModel();
    d->m_InteractionQueryModel = drugModel->drugInteractionQuery()->toStandardModel();

    d->ui->interactionQueryView->setModel(d->m_InteractionQueryModel);
    d->ui->interactionResultView->setModel(d->m_InteractionModel);
    d->ui->interactionResultView->expandAll();
    d->ui->interactionResultView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->ui->interactionQueryView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    d->ui->classgroup->hide();

    d->ui->tabWidget->setCurrentWidget(d->ui->tabInfo);

    // Create EBM view/model management
    d->m_TreeEbmModel = new MedicalUtils::EbmModel(this);
    d->ui->treeReferences->setModel(d->m_TreeEbmModel);
    d->ui->treeReferences->setModelColumn(MedicalUtils::EbmModel::ShortReferences);
    d->ui->treeReferences->setAlternatingRowColors(true);
    connect(d->ui->treeReferences, SIGNAL(activated(QModelIndex)), this, SLOT(showEbm(QModelIndex)));

    d->m_DdiEbmModel = new MedicalUtils::EbmModel(this);
    d->ui->ddiReferences->setModel(d->m_DdiEbmModel);
    d->ui->ddiReferences->setModelColumn(MedicalUtils::EbmModel::ShortReferences);
    d->ui->ddiReferences->setAlternatingRowColors(true);
    connect(d->ui->ddiReferences, SIGNAL(activated(QModelIndex)), this, SLOT(showEbm(QModelIndex)));
    d->ui->biblio->setReadOnly(true);

    connect(d->ui->interactionResultView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(interactionActivated(QModelIndex,QModelIndex)));
}

InteractionSynthesisDialog::~InteractionSynthesisDialog()
{
    if (d)
        delete d;
    d = 0;
}

// TODO: add class information
void InteractionSynthesisDialog::interactionActivated(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    Q_ASSERT(d->m_InteractionModel);
    Q_ASSERT(d->m_InteractionResult);
    if (!d->m_InteractionModel || !d->m_InteractionResult)
        return;

    const int id = d->m_InteractionModel->itemFromIndex(current)->data(Qt::UserRole).toInt();
    if (id==-1)
        return;
    if (id >= d->m_InteractionResult->interactions().count())
        return;

    d->ui->riskBrowser->clear();
    d->ui->managementBrowser->clear();
    d->ui->biblio->clear();
    d->m_TreeEbmModel->clear();
    d->m_DdiEbmModel->clear();

    DrugsDB::IDrugInteraction *interaction = d->m_InteractionResult->interactions().at(id);
    d->ui->riskBrowser->setPlainText(interaction->risk().replace("<br />","\n").replace("<br>","\n"));
    d->ui->managementBrowser->setPlainText(interaction->management().replace("<br />","\n").replace("<br>","\n"));
//    d->ui->link->setText(QString("<a href=\"%1\">Link to reference</a>").arg(interaction->referencesLink()));
    d->ui->getBiblio->setEnabled(true);
    d->populateDrugsBrowser(interaction, d->ui->interactingDrugsBrowser);
}

void InteractionSynthesisDialog::on_getBiblio_clicked()
{
    Q_ASSERT(d->m_InteractionModel);
    Q_ASSERT(d->m_InteractionResult);
    if (!d->m_InteractionModel || !d->m_InteractionResult)
        return;

    // Get the related DrugInteraction
    const int id = d->m_InteractionModel->itemFromIndex(d->ui->interactionResultView->selectionModel()->currentIndex())->data(Qt::UserRole).toInt();
    if (id==-1)
        return;
    if (id >= d->m_InteractionResult->interactions().count())
        return;
    d->ui->getBiblio->setEnabled(false);
    DrugsDB::IDrugInteraction *interaction = d->m_InteractionResult->interactions().at(id);

    // Populate models with EbmData
    if (d->m_Biblio.values(interaction).count()==0) {
        foreach(const DrugsDB::IDrug *drug, interaction->drugs()) {
            QVector<MedicalUtils::EbmData *> v = drugsBase().getAllBibliographyFromTree(drug->allInnAndInteractingClassesIds().toList());
            for(int i=0; i< v.count(); ++i) {
                MedicalUtils::EbmData *data = v.at(i);
                d->m_Biblio.insertMulti(interaction, data);
            }
        }
    }
    d->m_TreeEbmModel->setEbmData(d->m_Biblio.values(interaction).toVector());
}

void InteractionSynthesisDialog::showEbm(const QModelIndex &index)
{
    d->ui->biblio->setHtml(d->m_TreeEbmModel->index(index.row(), MedicalUtils::EbmModel::AbstractWithReferencesHtml).data().toString());
}

void InteractionSynthesisDialog::print(QAction *action)
{
    QVector<DrugsDB::IDrug *> drugs;
    QVector<DrugsDB::IDrugInteraction *> interactions;
    QString head;
    if (action == d->aPrintAll) {
        head = tr("Tested drugs");
        drugs = d->m_DrugModel->drugsList().toVector();
        interactions = d->m_InteractionResult->interactions();
    } else if (action == d->aPrintOne) {
        head = tr("Related to drugs");
        const int id = d->m_InteractionModel->itemFromIndex(d->ui->interactionResultView->selectionModel()->currentIndex())->data(Qt::UserRole).toInt();
        if (id==-1)
            return;
        if (id >= d->m_InteractionResult->interactions().count())
            return;
        interactions << d->m_InteractionResult->interactions().at(id);
   }
    // Prepare text to print
    QString display;
    display.append("<p align=center>" + head + "</p><p>");
    for(int i=0; i < drugs.count(); ++i) {
        display.append(QString("%1&nbsp;.&nbsp;%2<br />")
                       .arg(i)
                       .arg(drugs.at(i)->brandName()));
    }
    display.append("</p><p>");

    if (interactions.count() > 0) {
        display.append(DrugsDB::InteractionManager::synthesisToHtml(interactions, true));
    } else {
        display = tkTr(Trans::Constants::NO_1_FOUND).arg(tkTr(Trans::Constants::INTERACTION));
    }
    display.append("</p>");

    // Get IDocPrinter and print text
    Core::IDocumentPrinter *p = printer();
    if (!p) {
        Utils::Log::addError(this, "No IDocumentPrinter found", __FILE__, __LINE__);
        return;
    }
    p->clearTokens();
    QHash<QString, QVariant> tokens;
    tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, this->windowTitle());
    tokens.insert(Core::Constants::TOKEN_DATE, QDate::currentDate().toString("dd MMMM yyyy"));
    p->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);
    p->print(display, Core::IDocumentPrinter::Papers_Prescription_User, true);
}

void InteractionSynthesisDialog::drugReportRequested()
{
    // TODO: code here
}

void InteractionSynthesisDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
