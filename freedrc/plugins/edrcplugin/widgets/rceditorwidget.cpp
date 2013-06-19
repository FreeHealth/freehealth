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
#include <edrcplugin/models/rcitemmodel.h>
#include <edrcplugin/models/rctreemodel.h>
#include <edrcplugin/models/preventablecriticalriskmodel.h>
#include <edrcplugin/models/rccriteriasmodel.h>

#include "ui_rceditorwidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <QButtonGroup>
#include <QSortFilterProxyModel>

#include <QDebug>

using namespace eDRC;
using namespace Internal;

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
        _rcItemModel(0),
        _rcTreeModel(0),
        _pcrModel(0),
        _rcCritModel(0),
        _rcTreeProxy(0),
        _posDiagGroup(0),
        _aldGroup(0),
        _suiviGroup(0),
        _symptoGroup(0),
        q(parent)
    {
    }

    ~RcEditorWidgetPrivate()
    {
        delete ui;
    }

public:
    Ui::RcEditorWidget *ui;
    RCClassModel *_classModel;
    RcModel *_RcModel;
    RcItemModel *_rcItemModel;
    RcTreeModel *_rcTreeModel;
    PreventableCriticalRiskModel *_pcrModel;
    RcCriteriasModel *_rcCritModel;
    TreeProxyModel *_rcTreeProxy;
    QButtonGroup *_posDiagGroup, *_aldGroup, *_suiviGroup, *_symptoGroup;

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

    // Create the radio groups
    QButtonGroup *group = d->_posDiagGroup = new QButtonGroup(this);
    group->setExclusive(true);
    group->addButton(d->ui->radioDiagA);
    group->addButton(d->ui->radioDiagB);
    group->addButton(d->ui->radioDiagC);
    group->addButton(d->ui->radioDiagD);
    group->addButton(d->ui->radioDiagZ);

    group = d->_aldGroup = new QButtonGroup(this);
    group->setExclusive(true);
    group->addButton(d->ui->radioAldNon);
    group->addButton(d->ui->radioAldOui);

    group = d->_suiviGroup = new QButtonGroup(this);
    group->setExclusive(true);
    group->addButton(d->ui->radioSuiviN);
    group->addButton(d->ui->radioSuiviP);
    group->addButton(d->ui->radioSuiviR);

    group = d->_symptoGroup = new QButtonGroup(this);
    group->setExclusive(true);
    group->addButton(d->ui->radioSymptoOui);
    group->addButton(d->ui->radioSymptoNon);

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
    d->ui->searchLine->setEditorPlaceholderText("Rechercher un résultat de consultation");
    d->ui->searchLine->setLeftIcon(theme()->icon(Core::Constants::ICONSEARCH));
    connect(d->ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(onSearchTextChanged(QString)));

    // Create the RCItem && RcCriterias model/view
    d->_rcItemModel = new RcItemModel(this);
    d->_rcCritModel = new RcCriteriasModel(this);
    d->ui->listViewItems->setModel(d->_rcCritModel);
    d->ui->listViewItems->setModelColumn(RcCriteriasModel::Label);

    // Create the PreventableCriticalRisk model/view
    d->_pcrModel = new PreventableCriticalRiskModel(this);
    d->ui->pcrView->setModel(d->_pcrModel);
    d->ui->pcrView->setModelColumn(PreventableCriticalRiskModel::Label);

    connect(d->ui->SFMG, SIGNAL(clicked()), this, SLOT(onSmfgAboutClicked()));
    connect(d->ui->arguments, SIGNAL(clicked()), this, SLOT(onArgumentsClicked()));
}

RcEditorWidget::~RcEditorWidget()
{
}

/** When user activate a RC in the RC tree view. Update all models/views */
void RcEditorWidget::onCurrentRcChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    // Is the index a RC?
    if (current.parent() == QModelIndex()) {
        return;
    }

    disconnect(d->ui->treeViewRC->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentRcChanged(QModelIndex,QModelIndex)));

    // Update models
    int rcId = d->_rcTreeModel->id(d->_rcTreeProxy->mapToSource(current));
    d->_pcrModel->setFilterOnRcId(rcId);
    d->_rcItemModel->setFilterOnRcId(rcId);
    d->_rcCritModel->setFilterOnRcId(rcId);

    // Update some view
    const QStringList &autDiag = edrcBase().getRcAuthprizedDiagnosis(rcId, true);
    d->ui->radioDiagA->setVisible(autDiag.contains("A"));
    d->ui->radioDiagB->setVisible(autDiag.contains("B"));
    d->ui->radioDiagC->setVisible(autDiag.contains("C"));
    d->ui->radioDiagD->setVisible(autDiag.contains("D"));
    d->ui->radioDiagZ->setVisible(autDiag.contains("Z"));

    const QStringList &CIM10 = edrcBase().getRcIcd10RelatedCodes(rcId, true);
    d->ui->CIM10->setText(CIM10.join(" ; "));

    d->ui->arguments->setEnabled(true);

    connect(d->ui->treeViewRC->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentRcChanged(QModelIndex,QModelIndex)));
}

/** Slot connected to the user search line edition. Update the RC filtering */
void RcEditorWidget::onSearchTextChanged(const QString &text)
{
    d->_rcTreeProxy->setFilterKeyColumn(RcTreeModel::Label);
    d->_rcTreeProxy->setFilterFixedString(text);
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
    RcArgumentsDialog dlg(this);
    dlg.setRcId(rcId);
    dlg.exec();
    d->ui->arguments->setEnabled(true);
}
