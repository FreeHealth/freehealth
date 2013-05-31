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
 *  This program is distributed in the hope that it will be useful, *
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
#include "edrc_editor_widget.h"
#include "edrcbase.h"
#include "edrccore.h"
#include "rcclassmodel.h"
#include "rcmodel.h"
#include "rcitemmodel.h"
#include "rctreemodel.h"
#include "rcemodel.h"
#include "rccriteriasmodel.h"
#include "rcargumentsdialog.h"
#include "sfmgaboutdialog.h"

#include "ui_edrc_editor_widget.h"

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

class EdrcEditorWidgetPrivate
{
public:
    EdrcEditorWidgetPrivate(EdrcEditorWidget *parent):
        ui(new Ui::EdrcEditorWidget),
        _classModel(0),
        _rcModel(0),
        _rcItemModel(0),
        _rcTreeModel(0),
        _rceModel(0),
        _rcCritModel(0),
        _rcTreeProxy(0),
        _posDiagGroup(0),
        _aldGroup(0),
        _suiviGroup(0),
        _symptoGroup(0),
        q(parent)
    {
    }

    ~EdrcEditorWidgetPrivate()
    {
        delete ui;
    }

public:
    Ui::EdrcEditorWidget *ui;
    RCClassModel *_classModel;
    RCModel *_rcModel;
    RCItemModel *_rcItemModel;
    RcTreeModel *_rcTreeModel;
    RceModel *_rceModel;
    RcCriteriasModel *_rcCritModel;
    TreeProxyModel *_rcTreeProxy;
    QButtonGroup *_posDiagGroup, *_aldGroup, *_suiviGroup, *_symptoGroup;

private:
    EdrcEditorWidget *q;
};
} // namespace eDRC
} // namespace Internal

EdrcEditorWidget::EdrcEditorWidget(QWidget *parent) :
    QWidget(parent),
    d(new EdrcEditorWidgetPrivate(this))
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

    // Create the class model & populate the class combo
//    d->_classModel = new RCClassModel(this);
//    d->ui->classCombo->setModel(d->_classModel);
//    d->ui->classCombo->setModelColumn(RCClassModel::Label);
//    d->ui->classCombo->setToolTip("Classes de regroupement des résultats de consultation");

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

    d->ui->searchLine->setEditorPlaceholderText("Rechercher un résultat de consultation");
    d->ui->searchLine->setLeftIcon(theme()->icon(Core::Constants::ICONSEARCH));
    connect(d->ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(onSearchTextChanged(QString)));

    // Create the RCItem && RcCriterias model/view
    d->_rcItemModel = new RCItemModel(this);
    d->_rcCritModel = new RcCriteriasModel(this);
    d->ui->listViewItems->setModel(d->_rcCritModel);
    d->ui->listViewItems->setModelColumn(RcCriteriasModel::Label);

    // Create the RCE model/view
    d->_rceModel = new RceModel(this);
    d->ui->rceView->setModel(d->_rceModel);
    d->ui->rceView->setModelColumn(RceModel::Label);

    connect(d->ui->SFMG, SIGNAL(clicked()), this, SLOT(onSmfgAboutClicked()));
    connect(d->ui->arguments, SIGNAL(clicked()), this, SLOT(onArgumentsClicked()));
}

EdrcEditorWidget::~EdrcEditorWidget()
{
}

/**
 * When user activate a RC in the RC tree view. Update all models/views
 */
void EdrcEditorWidget::onCurrentRcChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    // Is the index a RC?
    if (current.parent() == QModelIndex()) {
        return;
    }

    disconnect(d->ui->treeViewRC->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentRcChanged(QModelIndex,QModelIndex)));

    // Update models
    int rcId = d->_rcTreeModel->id(d->_rcTreeProxy->mapToSource(current));
    d->_rceModel->setRcFilter(rcId);
    d->_rcItemModel->setFilterOnRcId(rcId);
    d->_rcCritModel->setFilterOnRcId(rcId);

    // Update some view
//    d->ui->argumentaire->setHtml(edrcBase().getRcArguments(rcId, true));
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

void EdrcEditorWidget::onSearchTextChanged(const QString &text)
{
    d->_rcTreeProxy->setFilterKeyColumn(RcTreeModel::Label);
    d->_rcTreeProxy->setFilterFixedString(text);
}

void EdrcEditorWidget::onSmfgAboutClicked()
{
    SfmgAboutDialog dlg(this);
    dlg.exec();
}

void EdrcEditorWidget::onArgumentsClicked()
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
