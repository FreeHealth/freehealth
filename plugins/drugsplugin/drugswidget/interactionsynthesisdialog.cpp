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
#include <drugsbaseplugin/interactionsmanager.h>
#include <drugsbaseplugin/drugsinteraction.h>
#include <drugsbaseplugin/drugsdata.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/idocumentprinter.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <extensionsystem/pluginmanager.h>

#include "ui_interactionsynthesisdialog.h"

#include <QToolButton>
#include <QToolBar>

using namespace DrugsWidget;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsModel *drugModel() { return DrugsDB::DrugsModel::activeModel(); }
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}


namespace DrugsWidget {
namespace Internal {
class InteractionSynthesisDialogPrivate
{
public:
    QList<DrugsDB::Internal::DrugsInteraction *> m_Interactions;
    QAction *aPrint;
};
}
}

InteractionSynthesisDialog::InteractionSynthesisDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InteractionSynthesisDialog),
    d(new Internal::InteractionSynthesisDialogPrivate)
{
    // get interactions
    d->m_Interactions = drugModel()->interactionsManager()->getAllInteractionsFound();

    // create and populate the views
    ui->setupUi(this);
    setWindowTitle(tr("Synthetic interactions") + " - " + qApp->applicationName());
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);

    connect(ui->interactors, SIGNAL(itemActivated(QTableWidgetItem*)), this, SLOT(interactorsActivated(QTableWidgetItem*)));
    connect(ui->interactors->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(interactorsActivated(QModelIndex,QModelIndex)));

    ui->interactors->setAlternatingRowColors(true);
    ui->firstClassInfos->hide();
    ui->secondClassInfos->hide();

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
    a->setText(tkTr(Trans::Constants::PRECAUTION_FOR_USE));
    a->setIcon(theme()->icon(DrugsDB::Constants::INTERACTION_ICONPRECAUTION, Core::ITheme::MediumIcon));
    a->setData(3);
    bar->addAction(a);

    a = new QAction(this);
    a->setText(tkTr(Trans::Constants::TAKE_INTO_ACCOUNT));
    a->setIcon(theme()->icon(DrugsDB::Constants::INTERACTION_ICONTAKEINTOACCOUNT, Core::ITheme::MediumIcon));
    a->setData(4);
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

void InteractionSynthesisDialog::levelActivated(QAction *a)
{
    if (!a)
        return;

    int level = a->data().toInt();
    if (level==-1)
        return;

    switch (level) {
    case 0: level = 0; break;
    case 1: level = DrugsDB::Constants::Interaction::ContreIndication; break;
    case 2: level = DrugsDB::Constants::Interaction::Deconseille; break;
    case 3: level = DrugsDB::Constants::Interaction::Precaution; break;
    case 4: level = DrugsDB::Constants::Interaction::APrendreEnCompte; break;
    case 5: level = DrugsDB::Constants::Interaction::Information; break;
    default: level = 0;
    }
    ui->interactors->blockSignals(true);
    ui->interactors->selectionModel()->blockSignals(true);
    ui->interactors->clear();
    ui->interactors->setRowCount(0);
    ui->interactors->setColumnCount(3);
    ui->interactors->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
    ui->interactors->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    ui->interactors->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    ui->interactors->horizontalHeader()->hide();
    ui->interactors->setColumnWidth(0, 24);
    int row = 0;
    foreach(DrugsDB::Internal::DrugsInteraction *interaction, d->m_Interactions) {
        if ((interaction->type() & level) || (level == 0)) {
            ui->interactors->insertRow(row);
            QTableWidgetItem *icon = new QTableWidgetItem(DrugsDB::InteractionsManager::interactionIcon(interaction->type()), "");
            icon->setData(Qt::UserRole, d->m_Interactions.indexOf(interaction));
            QTableWidgetItem *atc1 = new QTableWidgetItem(interaction->value(DrugsDB::Internal::DrugsInteraction::DI_ATC1_Label).toString());
            atc1->setData(Qt::UserRole, d->m_Interactions.indexOf(interaction));
            QTableWidgetItem *atc2 = new QTableWidgetItem(interaction->value(DrugsDB::Internal::DrugsInteraction::DI_ATC2_Label).toString());
            atc2->setData(Qt::UserRole, d->m_Interactions.indexOf(interaction));
            ui->interactors->setItem(row,0, icon);
            ui->interactors->setItem(row,1, atc1);
            ui->interactors->setItem(row,2, atc2);
        }
    }
    ui->interactors->blockSignals(false);
    ui->interactors->selectionModel()->blockSignals(false);
    ui->interactors->selectRow(0);
}

/** \todo add class informations */
void InteractionSynthesisDialog::interactorsActivated(QTableWidgetItem *item)
{
    ui->riskBrowser->clear();
    ui->managementBrowser->clear();
    int id = item->data(Qt::UserRole).toInt();
    if (id >= d->m_Interactions.count())
        return;
    DrugsDB::Internal::DrugsInteraction *interaction = d->m_Interactions.at(id);
    ui->riskBrowser->setPlainText(interaction->information().replace("<br>","\n"));
    ui->managementBrowser->setPlainText(interaction->whatToDo().replace("<br>","\n"));
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
    int id = item->data(Qt::UserRole).toInt();
    if (id >= d->m_Interactions.count())
        return;
    DrugsDB::Internal::DrugsInteraction *interaction = d->m_Interactions.at(id);
    ui->riskBrowser->setPlainText(interaction->information().replace("<br>","\n"));
    ui->managementBrowser->setPlainText(interaction->whatToDo().replace("<br>","\n"));
}

void InteractionSynthesisDialog::print()
{
    // Prepare text to print
    QString display;
    int i = 0;
    display.append("<p>");
    foreach(DrugsDB::Internal::DrugsData *drg, DrugsDB::DrugsModel::activeModel()->drugsList()) {
        ++i;
        display.append(QString("%1&nbsp;.&nbsp;%2<br />")
                       .arg(i)
                       .arg(drg->denomination()));
    }
    display.append("</p><p>");
    if (d->m_Interactions.count() > 0) {
        display.append(DrugsDB::InteractionsManager::synthesisToHtml(d->m_Interactions, true));
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
