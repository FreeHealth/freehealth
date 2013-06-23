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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "rceditorwidget.h"
#include "sfmgaboutdialog.h"
#include "rcargumentsdialog.h"

#include <edrcplugin/edrccore.h>
#include <edrcplugin/database/edrcbase.h>
#include <edrcplugin/models/rcclassmodel.h>
#include <edrcplugin/models/rcmodel.h>
#include <edrcplugin/models/rctreemodel.h>
#include <edrcplugin/models/preventablecriticalriskmodel.h>
#include <edrcplugin/models/rccriteriasmodel.h>

#include "ui_rceditorwidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QButtonGroup>
#include <QSortFilterProxyModel>

#include <QDebug>

using namespace eDRC;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline eDRC::EdrcCore &edrcCore() {return eDRC::EdrcCore::instance();}
static inline eDRC::Internal::DrcDatabase &edrcBase() {return eDRC::EdrcCore::instance().edrcBase();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

namespace eDRC {
namespace Internal {
class TreeProxyModel : public QSortFilterProxyModel
{
public:
    TreeProxyModel(QObject *parent = 0)
        : QSortFilterProxyModel(parent)
    {
        setFilterCaseSensitivity(Qt::CaseInsensitive);
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
    {
        if (filterRegExp().isEmpty())
            return true;

        // Force to cycle throw the first column, because other columns does not have children
        QModelIndex currentParent(sourceModel()->index(sourceRow, 0, sourceParent));
        QModelIndex currentToFilter(sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent));

        if (sourceModel()->data(currentToFilter).toString().contains(filterRegExp()))
            return true;

        if (sourceModel()->hasChildren(currentParent)) {
            bool atLeastOneValidChild = false;
            int i = 0;
            while (!atLeastOneValidChild) {
                const QModelIndex child(currentParent.child(i, currentParent.column()));
                if (!child.isValid())
                    // No valid child
                    break;

                atLeastOneValidChild = filterAcceptsRow(i, currentParent);
                i++;
            }
            return atLeastOneValidChild;
        }

        return false;
    }
};

class RcEditorWidgetPrivate
{
public:
    RcEditorWidgetPrivate(RcEditorWidget *parent):
        ui(new Ui::RcEditorWidget),
        _classModel(0),
        _RcModel(0),
        _rcTreeModel(0),
        _pcrModel(0),
        _rcCritModel(0),
        _rcTreeProxy(0),
        _posDiagGroup(0),
        _suiviGroup(0),
        q(parent)
    {
    }

    ~RcEditorWidgetPrivate()
    {
        delete ui;
    }

    void manageRadioButtons()
    {
        QButtonGroup *group = _posDiagGroup = new QButtonGroup(q);
        group->setExclusive(true);
        group->addButton(ui->buttonA);
        group->addButton(ui->buttonB);
        group->addButton(ui->buttonC);
        group->addButton(ui->buttonD);
        group->addButton(ui->buttonZ);

        group = _suiviGroup = new QButtonGroup(q);
        group->setExclusive(true);
        group->addButton(ui->buttonN);
        group->addButton(ui->buttonP);
        group->addButton(ui->buttonR);
    }

    void updateCodingStatus()
    {
        // Compute the current CR coding status
        QStringList tooltip;
        bool codingIsOk = true;

        // Check diagnosis position & follow up
        if (!_posDiagGroup->checkedButton()) {
            tooltip << QApplication::translate("eDRC::Internal::RcEditorWidget", "No diagnosis position code selected");
            codingIsOk = false;
        } else {
            tooltip << QApplication::translate("eDRC::Internal::RcEditorWidget", "Diagnosis position: %1").arg(_posDiagGroup->checkedButton()->text());
        }
        if (!_suiviGroup->checkedButton()) {
            tooltip << QApplication::translate("eDRC::Internal::RcEditorWidget", "No follow up code selected");
            codingIsOk = false;
        } else {
            tooltip << QApplication::translate("eDRC::Internal::RcEditorWidget", "Follow up: %1").arg(_suiviGroup->checkedButton()->text());
        }
        tooltip << QApplication::translate("eDRC::Internal::RcEditorWidget", "Chronic disease: %1").arg(ui->buttonALD->isChecked()? tkTr(Trans::Constants::YES):tkTr(Trans::Constants::NO));
        tooltip << QApplication::translate("eDRC::Internal::RcEditorWidget", "Symptomatic: %1").arg(ui->buttonSympto->isChecked()? tkTr(Trans::Constants::YES):tkTr(Trans::Constants::NO));

        // Manage coding completion labels
        switch (_rcCritModel->currentCodingStatus()) {
        case RcCriteriasModel::ValidCoding:
            tooltip << QApplication::translate("eDRC::Internal::RcEditorWidget", "Valid criteria coding");
            break;
        case RcCriteriasModel::IncompleteCoding:
            tooltip << QApplication::translate("eDRC::Internal::RcEditorWidget", "Incomplete or incorrect coding of the criterias");
            codingIsOk = false;
            break;
        case RcCriteriasModel::NoCodingStarted:
            tooltip << QApplication::translate("eDRC::Internal::RcEditorWidget", "No criteria selected");
            codingIsOk = false;
            break;
        }

        if (codingIsOk)
            ui->codingResultIcon->setPixmap(theme()->icon(Core::Constants::ICONOK).pixmap(16,16));
        else
            ui->codingResultIcon->setPixmap(theme()->icon(Core::Constants::ICONWARNING).pixmap(16,16));

        ui->codingResultIcon->setToolTip(tooltip.join("\n"));
    }

public:
    Ui::RcEditorWidget *ui;
    RCClassModel *_classModel;
    RcModel *_RcModel;
    RcTreeModel *_rcTreeModel;
    PreventableCriticalRiskModel *_pcrModel;
    RcCriteriasModel *_rcCritModel;
    TreeProxyModel *_rcTreeProxy;
    QButtonGroup *_posDiagGroup, *_suiviGroup;

private:
    RcEditorWidget *q;
};
} // namespace eDRC
} // namespace Internal

RcEditorWidget::RcEditorWidget(QWidget *parent) :
    QWidget(parent),
    d(new RcEditorWidgetPrivate(this))
{
    d->ui->setupUi(this);
    d->ui->SFMG->setIcon(theme()->icon("sfmg_logo.png", Core::ITheme::SmallIcon));
    d->ui->arguments->setEnabled(false);

    // Manage splitters
    d->ui->searchSplitter->setStretchFactor(0, 2);
    d->ui->searchSplitter->setStretchFactor(1, 1);
    d->ui->centralSplitter->setStretchFactor(0, 1);
    d->ui->centralSplitter->setStretchFactor(1, 3);
    d->ui->commentSplitter->setStretchFactor(0, 3);
    d->ui->commentSplitter->setStretchFactor(1, 1);

    // Create the radio groups
    d->manageRadioButtons();
    connect(d->_posDiagGroup, SIGNAL(buttonClicked(int)), this, SLOT(updateCodingStatus()));
    connect(d->_suiviGroup, SIGNAL(buttonClicked(int)), this, SLOT(updateCodingStatus()));

    //  Creating RcTreeModel/view
    d->_rcTreeModel = new RcTreeModel(this);
    d->_rcTreeModel->initialize();
    d->_rcTreeProxy = new TreeProxyModel(this);
    d->_rcTreeProxy->setSourceModel(d->_rcTreeModel);
    d->ui->treeViewRC->setModel(d->_rcTreeProxy);
    d->ui->treeViewRC->header()->setSectionHidden(RcTreeModel::Id, true);
    d->ui->treeViewRC->header()->hide();
    d->ui->treeViewRC->setIndentation(12);
    connect(d->ui->treeViewRC->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentRcChanged(QModelIndex,QModelIndex)));

    // Creating the search line edit
    d->ui->searchLine->setEditorPlaceholderText(tr("Search a result of consultation"));
    d->ui->searchLine->setLeftIcon(theme()->icon(Core::Constants::ICONSEARCH));
    connect(d->ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(onSearchTextChanged(QString)));

    // Create the RCItem && RcCriterias model/view
    d->_rcCritModel = new RcCriteriasModel(this);
    d->ui->listViewItems->setModel(d->_rcCritModel);
    d->ui->listViewItems->setModelColumn(RcCriteriasModel::Label);
    connect(d->ui->listViewItems, SIGNAL(pressed(QModelIndex)), this, SLOT(onCriteriaItemPressed(QModelIndex)));

    // Create the PreventableCriticalRisk model/view
    d->_pcrModel = new PreventableCriticalRiskModel(this);
    d->ui->pcrView->setModel(d->_pcrModel);
    d->ui->pcrView->setModelColumn(PreventableCriticalRiskModel::Label);

    connect(d->ui->SFMG, SIGNAL(clicked()), this, SLOT(onSmfgAboutClicked()));
    connect(d->ui->arguments, SIGNAL(clicked()), this, SLOT(onArgumentsClicked()));
}

RcEditorWidget::~RcEditorWidget()
{
    if (d)
        delete d;
    d = 0;
}

/** When user activate a RC in the RC tree view. Update all models/views */
void RcEditorWidget::onCurrentRcChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    // Is the index a RC?
    if (current.parent() == QModelIndex()) {
        return;
    }

    // Disconnect the tree view
    disconnect(d->ui->treeViewRC->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentRcChanged(QModelIndex,QModelIndex)));

    // Update models
    int rcId = d->_rcTreeModel->id(d->_rcTreeProxy->mapToSource(current));
    d->_pcrModel->setFilterOnRcId(rcId);
    d->_rcCritModel->setFilterOnRcId(rcId);

    // Update buttons & status label
    const QStringList &autDiag = edrcBase().getRcAuthprizedDiagnosis(rcId, true);
    d->ui->buttonA->setVisible(autDiag.contains("A"));
    d->ui->buttonB->setVisible(autDiag.contains("B"));
    d->ui->buttonC->setVisible(autDiag.contains("C"));
    d->ui->buttonD->setVisible(autDiag.contains("D"));
    d->ui->buttonZ->setVisible(autDiag.contains("Z"));

    d->_posDiagGroup->setExclusive(false);
    d->ui->buttonA->setChecked(false);
    d->ui->buttonB->setChecked(false);
    d->ui->buttonC->setChecked(false);
    d->ui->buttonD->setChecked(false);
    d->ui->buttonZ->setChecked(false);
    d->_posDiagGroup->setExclusive(true);

    d->_suiviGroup->setExclusive(false);
    d->ui->buttonN->setChecked(false);
    d->ui->buttonP->setChecked(false);
    d->ui->buttonR->setChecked(false);
    d->_suiviGroup->setExclusive(true);

    d->ui->buttonALD->setChecked(false);
    d->ui->buttonSympto->setChecked(false);
    d->updateCodingStatus();

    // Update the ICD10 code view
    const QStringList &CIM10 = edrcBase().getRcIcd10RelatedCodes(rcId, true);
    d->ui->CIM10->setText(CIM10.join(" ; "));

    // Update argument button
    d->ui->arguments->setEnabled(true);

    // Reconnect the treeview
    connect(d->ui->treeViewRC->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentRcChanged(QModelIndex,QModelIndex)));
}

/** Slot connected to the user search line edition. Update the RC filtering */
void RcEditorWidget::onSearchTextChanged(const QString &text)
{
    // Filter the proxymodel
    d->_rcTreeProxy->setFilterKeyColumn(RcTreeModel::Label);
    d->_rcTreeProxy->setFilterFixedString(text);

    // Count the number of root indexes & expand all if <= 5
    if (d->_rcTreeProxy->rowCount(QModelIndex()) <= 5) {
        d->ui->treeViewRC->expandToDepth(0);
    }
}

/**
 * When a CR criteria is switched from unselected to selected, we need to check
 * if the criteria has a parent (using its 'ponderation') and select it if required.
 */
void RcEditorWidget::onCriteriaItemPressed(const QModelIndex &index)
{
    // Inform the criteria model
    d->_rcCritModel->setData(index, -1, Qt::CheckStateRole);

    // Update coding status label
    d->updateCodingStatus();
}

/** Open the SFMG about dialog */
void RcEditorWidget::onSmfgAboutClicked()
{
    SfmgAboutDialog dlg(this);
    dlg.exec();
}

/** Open the argument dialog for the currently selected RC */
void RcEditorWidget::onArgumentsClicked()
{
    d->ui->arguments->setEnabled(false);
    QModelIndex current = d->ui->treeViewRC->currentIndex();
    // Is the index a RC?
    if (current.parent() == QModelIndex()) {
        return;
    }

    int rcId = d->_rcTreeModel->id(d->_rcTreeProxy->mapToSource(current));
    RcArgumentsDialog dlg(this->parentWidget());
    dlg.setRcId(rcId);
    dlg.exec();
    d->ui->arguments->setEnabled(true);
}

/** Update the CR coding status label */
void RcEditorWidget::updateCodingStatus()
{
    d->updateCodingStatus();
}

void RcEditorWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        d->ui->retranslateUi(this);
        d->updateCodingStatus();
    }
}
