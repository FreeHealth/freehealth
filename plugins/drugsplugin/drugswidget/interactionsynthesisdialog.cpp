#include "interactionsynthesisdialog.h"

#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/interactionsmanager.h>
#include <drugsbaseplugin/drugsinteraction.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include "ui_interactionsynthesisdialog.h"

#include <QToolButton>
#include <QToolBar>

using namespace DrugsWidget;
//using namespace DrugsWidget::Constants;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsModel *drugModel() { return DrugsDB::DrugsModel::activeModel(); }
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}


namespace DrugsWidget {
namespace Internal {
class InteractionSynthesisDialogPrivate
{
public:
    QList<DrugsDB::Internal::DrugsInteraction *> m_Interactions;
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

    ui->interactors->setAlternatingRowColors(true);

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

    connect(bar, SIGNAL(actionTriggered(QAction*)), this, SLOT(levelActivated(QAction*)));
    connect(ui->interactors, SIGNAL(itemActivated(QTableWidgetItem*)), this, SLOT(interactorsActivated(QTableWidgetItem*)));
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
    switch (level) {
    case 0: level = 0; break;
    case 1: level = DrugsDB::Constants::Interaction::ContreIndication; break;
    case 2: level = DrugsDB::Constants::Interaction::Deconseille; break;
    case 3: level = DrugsDB::Constants::Interaction::Precaution; break;
    case 4: level = DrugsDB::Constants::Interaction::APrendreEnCompte; break;
    case 5: level = DrugsDB::Constants::Interaction::Information; break;
    default: level = 0;
    }
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
        if (interaction->type() == level || level == 0) {
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
}

void InteractionSynthesisDialog::interactorsActivated(QTableWidgetItem *item)
{
    ui->riskBrowser->clear();
    ui->managementBrowser->clear();
    int id = item->data(Qt::UserRole).toInt();
    if (id >= d->m_Interactions.count())
        return;
    DrugsDB::Internal::DrugsInteraction *interaction = d->m_Interactions.at(id);
    ui->riskBrowser->setHtml(interaction->information());
    ui->managementBrowser->setHtml(interaction->whatToDo());
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
