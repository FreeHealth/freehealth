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
#include "interactionsynthesisdialog.h"

#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/interactionmanager.h>
#include <drugsbaseplugin/idruginteraction.h>
#include <drugsbaseplugin/drugsbase.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/idocumentprinter.h>

#include <utils/log.h>
#include <medicalutils/ebmdata.h>
#include <translationutils/constanttranslations.h>

#include <extensionsystem/pluginmanager.h>

#include "ui_interactionsynthesisdialog.h"

#include <QToolButton>
#include <QToolBar>
#include <QMultiHash>

using namespace DrugsWidget;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsModel *drugModel() { return DrugsDB::DrugsModel::activeModel(); }
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}
static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}


namespace DrugsWidget {
namespace Internal {
class InteractionSynthesisDialogPrivate
{
public:
    void populateDrugsBrowser(DrugsDB::IDrugInteraction *interaction, QTextBrowser *browser)
    {
        browser->clear();
        QString drugs;
        if (interaction->drugs().count() == 2) {
            /** \todo code here, move this in drugsbaseplugin ? */
//            DrugsDB::IDrug *drug1 = interaction->drugs().at(0);
//            DrugsDB::IDrug *drug2 = interaction->drugs().at(1);
//            int id1 = interaction->value(DrugsDB::IDrugInteraction::DI_ATC1).toInt();
//            int id2 = interaction->value(DrugsDB::IDrugInteraction::DI_ATC2).toInt();
//            if (!drug1->allInnAndIamClasses().contains(id1)) {
//                qSwap(id1, id2);
//            }
//            drugs += QString("<p>%1<br />&nbsp;&nbsp;&nbsp;&nbsp;%2</p>")
//                     .arg(drug1->brandName())
//                     .arg(drugsBase()->getAtcLabel(id1));
//            drugs += QString("<p>%1<br />&nbsp;&nbsp;&nbsp;&nbsp;%2</p>")
//                     .arg(drug2->brandName())
//                     .arg(drugsBase()->getAtcLabel(id2));
        }
        browser->setHtml(drugs);
    }

public:
    QList<DrugsDB::IDrugInteraction *> m_Interactions;
    QAction *aPrint;
    QMultiHash<DrugsDB::IDrugInteraction *, MedicalUtils::EbmData *> m_Biblio;
};
}
}

InteractionSynthesisDialog::InteractionSynthesisDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InteractionSynthesisDialog),
    d(new Internal::InteractionSynthesisDialogPrivate)
{
    // get interactions
    /** \todo code here */
//    d->m_Interactions = drugModel()->interactionsManager()->getAllInteractionsFound();

    // create and populate the views
    ui->setupUi(this);
    setWindowTitle(tr("Synthetic interactions") + " - " + qApp->applicationName());
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);

    connect(ui->interactors, SIGNAL(itemActivated(QTableWidgetItem*)), this, SLOT(interactorsActivated(QTableWidgetItem*)));
    connect(ui->interactors->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(interactorsActivated(QModelIndex,QModelIndex)));

    ui->interactors->setAlternatingRowColors(true);
    ui->classgroup->hide();
    ui->bibliogroup->hide();

    QToolBar *bar = new QToolBar(this);
    bar->setIconSize(QSize(32,32));
    bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->toolbarLayout->addWidget(bar);

    QAction *a;
    a = new QAction(this);
    a->setText(tr("All interactions"));
    a->setIcon(theme()->icon(DrugsDB::Constants::INTERACTION_ICONUNKONW, Core::ITheme::MediumIcon));
    a->setData(0);
    bar->addAction(a);
    levelActivated(a);

    a = new QAction(this);
    a->setText(tkTr(Trans::Constants::CONTRAINDICATION));
    a->setIcon(theme()->icon(DrugsDB::Constants::INTERACTION_ICONCRITICAL, Core::ITheme::MediumIcon));
    a->setData(1);
    bar->addAction(a);

    a = new QAction(this);
    a->setText(tkTr(Trans::Constants::DISCOURAGED));
    a->setIcon(theme()->icon(DrugsDB::Constants::INTERACTION_ICONDECONSEILLEE, Core::ITheme::MediumIcon));
    a->setData(2);
    bar->addAction(a);

    a = new QAction(this);
    a->setText(tkTr(Trans::Constants::TAKE_INTO_ACCOUNT));
    a->setIcon(theme()->icon(DrugsDB::Constants::INTERACTION_ICONTAKEINTOACCOUNT, Core::ITheme::MediumIcon));
    a->setData(4);
    bar->addAction(a);

    a = new QAction(this);
    a->setText(tkTr(Trans::Constants::PRECAUTION_FOR_USE));
    a->setIcon(theme()->icon(DrugsDB::Constants::INTERACTION_ICONPRECAUTION, Core::ITheme::MediumIcon));
    a->setData(3);
    bar->addAction(a);

    a = new QAction(this);
    a->setText(tkTr(Trans::Constants::INFORMATION));
    a->setIcon(theme()->icon(DrugsDB::Constants::INTERACTION_ICONINFORMATION, Core::ITheme::MediumIcon));
    a->setData(5);
    bar->addAction(a);

    d->aPrint = new QAction(this);
    d->aPrint->setText(tkTr(Trans::Constants::FILEPRINT_TEXT));
    d->aPrint->setShortcut(QKeySequence::Print);
    d->aPrint->setIcon(theme()->icon(Core::Constants::ICONPRINT, Core::ITheme::MediumIcon));
    d->aPrint->setData(-1);
    bar->addAction(d->aPrint);

    connect(bar, SIGNAL(actionTriggered(QAction*)), this, SLOT(levelActivated(QAction*)));
    connect(d->aPrint, SIGNAL(triggered()), this, SLOT(print()));
}

InteractionSynthesisDialog::~InteractionSynthesisDialog()
{
    delete ui;
}

static void addInteractionToView(int row, QTableWidget *tw, DrugsDB::IDrugInteraction *interaction, int interactionId)
{
    /** \todo code here */
//    tw->insertRow(row);
//    QTableWidgetItem *icon = new QTableWidgetItem(DrugsDB::InteractionManager::interactionIcon(interaction->type()), "");
//    icon->setData(Qt::UserRole, interactionId);
//    QTableWidgetItem *atc1 = new QTableWidgetItem(interaction->value(DrugsDB::IDrugInteraction::DI_ATC1_Label).toString());
//    atc1->setData(Qt::UserRole, interactionId);
//    QTableWidgetItem *atc2 = new QTableWidgetItem(interaction->value(DrugsDB::IDrugInteraction::DI_ATC2_Label).toString());
//    atc2->setData(Qt::UserRole, interactionId);
//    tw->setItem(row, 0, icon);
//    tw->setItem(row, 1, atc1);
//    tw->setItem(row, 2, atc2);
}

void InteractionSynthesisDialog::levelActivated(QAction *a)
{
    if (!a)
        return;

    int level = a->data().toInt();
    if (level==-1)
        return;

    /** \todo code here */
//    switch (level) {
//    case 0: level = 0; break;
//    case 1: level = DrugsDB::Constants::Interaction::ContreIndication; break;
//    case 2: level = DrugsDB::Constants::Interaction::Deconseille; break;
//    case 3: level = DrugsDB::Constants::Interaction::Precaution; break;
//    case 4: level = DrugsDB::Constants::Interaction::APrendreEnCompte; break;
//    case 5: level = DrugsDB::Constants::Interaction::Information; break;
//    default: level = 0;
//    }
//    ui->interactors->blockSignals(true);
//    ui->interactors->selectionModel()->blockSignals(true);
//    ui->riskBrowser->clear();
//    ui->managementBrowser->clear();
//    ui->interactingDrugsBrowser->clear();
//    ui->interactors->clear();
//    ui->interactors->setRowCount(0);
//    ui->interactors->setColumnCount(3);
//    ui->interactors->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
//    ui->interactors->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
//    ui->interactors->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
//    ui->interactors->horizontalHeader()->hide();
//    ui->interactors->setColumnWidth(0, 24);
    int row = 0;
    /** \todo code here */
//    foreach(DrugsDB::IDrugInteraction *interaction, d->m_Interactions) {
//        if (level == DrugsDB::Constants::Interaction::Information && interaction->type() & DrugsDB::Constants::Interaction::InnDuplication) {
//                addInteractionToView(row, ui->interactors, interaction, d->m_Interactions.indexOf(interaction));
//        }
//        if ((interaction->type() & level) || (level == 0)) {
//            addInteractionToView(row, ui->interactors, interaction, d->m_Interactions.indexOf(interaction));
//        }
//    }
    ui->interactors->blockSignals(false);
    ui->interactors->selectionModel()->blockSignals(false);
    ui->interactors->selectRow(0);
}

/** \todo add class informations */
void InteractionSynthesisDialog::interactorsActivated(QTableWidgetItem *item)
{
    ui->riskBrowser->clear();
    ui->managementBrowser->clear();
    ui->biblio->clear();
    ui->biblioReferences->clear();
    ui->interactingDrugsBrowser->clear();
    int id = item->data(Qt::UserRole).toInt();
    if (id >= d->m_Interactions.count())
        return;
    DrugsDB::IDrugInteraction *interaction = d->m_Interactions.at(id);
    ui->riskBrowser->setPlainText(interaction->risk().replace("<br />","\n"));
    ui->managementBrowser->setPlainText(interaction->management().replace("<br />","\n"));
    ui->link->setText(QString("<a href=\"%1\">Link to reference</a>").arg(interaction->referencesLink()));
    ui->getBiblio->setEnabled(true);
    d->populateDrugsBrowser(interaction, ui->interactingDrugsBrowser);
}

/** \todo add class informations */
void InteractionSynthesisDialog::interactorsActivated(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
    QTableWidgetItem *item = ui->interactors->currentItem();
    if (!item)
        return;
    ui->riskBrowser->clear();
    ui->managementBrowser->clear();
    ui->biblio->clear();
    ui->biblioReferences->clear();
    int id = item->data(Qt::UserRole).toInt();
    if (id >= d->m_Interactions.count())
        return;
    DrugsDB::IDrugInteraction *interaction = d->m_Interactions.at(id);
    ui->riskBrowser->setPlainText(interaction->risk().replace("<br />","\n"));
    ui->managementBrowser->setPlainText(interaction->management().replace("<br />","\n"));
    ui->link->setText(QString("<a href=\"%1\">Link to reference</a>").arg(interaction->referencesLink()));
    ui->getBiblio->setEnabled(true);
    d->populateDrugsBrowser(interaction, ui->interactingDrugsBrowser);
}

void InteractionSynthesisDialog::on_getBiblio_clicked()
{
    ui->getBiblio->setEnabled(false);
    QTableWidgetItem *item = ui->interactors->currentItem();
    if (!item)
        return;
    int id = item->data(Qt::UserRole).toInt();
    if (id >= d->m_Interactions.count())
        return;
    DrugsDB::IDrugInteraction *interaction = d->m_Interactions.at(id);
    bool show = false;
    if (d->m_Biblio.values(interaction).count()==0) {
        foreach(const DrugsDB::IDrug *drug, interaction->drugs()) {
            QVector<MedicalUtils::EbmData *> v = DrugsDB::Internal::DrugsBase::instance()->getAllSourcesFromTree(drug->allInnAndInteractingClassesIds().toList());
            foreach(MedicalUtils::EbmData *data, v) {
                d->m_Biblio.insertMulti(interaction, data);
            }
        }
    }

    QString reftable = "<table width=100% border=1>";
    QString bibtable = "<table width=100% border=1>";
    if (d->m_Biblio.values(interaction).count()==0) {
        reftable += QString("<tr><td>%1</td></tr>").arg(tr("No bibliography available"));
        bibtable += QString("<tr><td>%1</td></tr>").arg(tr("No bibliography available"));
    } else {
        foreach(MedicalUtils::EbmData *data, d->m_Biblio.values(interaction)) {
            show = true;
            QString link = data->link();
            link.replace("http://www.ncbi.nlm.nih.gov/pubmed/", "PMID ");
            reftable += QString("<tr><td width=70%>%1</td><td width=30%><a href='%2'>%2</a></td></tr>")
                        .arg(data->references())
                        .arg(link);
            bibtable += QString("<tr><td>%1</td></tr>")
                        .arg(data->abstract());
        }
    }
    reftable += "</table>";
    bibtable += "</table>";
    ui->biblio->setHtml(bibtable.replace("\n","<br />"));
    ui->biblioReferences->setHtml(reftable.replace("\n","<br />"));
    ui->bibliogroup->setVisible(show);
}

void InteractionSynthesisDialog::print()
{
    // Prepare text to print
    QString display;
    int i = 0;
    display.append("<p>");
    foreach(DrugsDB::IDrug *drg, DrugsDB::DrugsModel::activeModel()->drugsList()) {
        ++i;
        display.append(QString("%1&nbsp;.&nbsp;%2<br />")
                       .arg(i)
                       .arg(drg->brandName()));
    }
    display.append("</p><p>");
    if (d->m_Interactions.count() > 0) {
        /** \todo code here */
//        display.append(DrugsDB::InteractionManager::synthesisToHtml(d->m_Interactions, true));
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

void InteractionSynthesisDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
