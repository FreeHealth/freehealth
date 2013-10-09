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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class DDI::AtcCollectionEditorWidget
 * ATC classification visualisation page.
 */

#include "atccollectioneditorwidget.h"
#include "atctablemodel.h"
//#include "atctreemodel.h"
#include <ddiplugin/ddicore.h>
#include <ddiplugin/atc/atctreeproxymodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/widgets/detailswidget.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>

#include "ui_atccollectioneditorwidget.h"

#include <QLabel>
#include <QFormLayout>
#include <QLineEdit>
#include <QToolButton>

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DDI::DDICore *dbCore() {return DDI::DDICore::instance();}

namespace DDI {
namespace Internal {
class TreeProxyModel : public QSortFilterProxyModel
{
public:
    explicit TreeProxyModel(QObject *parent = 0);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool filterAcceptsRowItself(int source_row, const QModelIndex &source_parent) const;
    bool hasAcceptedChildren(int source_row, const QModelIndex &source_parent) const;
};

class AtcCollectionEditorWidgetPrivate
{
public:
    AtcCollectionEditorWidgetPrivate(AtcCollectionEditorWidget *parent):
        ui(0),
        _proxyModel(0),
        _atcTreeProxyModel(0),
        _left(0),
        aSearchEnglish(0),
        aSearchCode(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~AtcCollectionEditorWidgetPrivate()
    {
        delete ui;
    }

public:
    Ui::AtcCollectionEditorWidget *ui;
    TreeProxyModel *_proxyModel;
    AtcTreeProxyModel *_atcTreeProxyModel;
    QToolButton *_left;
    QAction *aSearchEnglish, *aSearchCode;

private:
    AtcCollectionEditorWidget *q;
};
} // namespace Internal
} // namespace DDI

TreeProxyModel::TreeProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

bool TreeProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (filterAcceptsRowItself(source_row, source_parent))
        return true;

    //accept if any of the parents is accepted on it's own merits
    QModelIndex parent = source_parent;
    while (parent.isValid()) {
        if (filterAcceptsRowItself(parent.row(), parent.parent()))
            return true;
        parent = parent.parent();
    }

    //accept if any of the children is accepted on it's own merits
    if (hasAcceptedChildren(source_row, source_parent)) {
        return true;
    }

    return false;
}

bool TreeProxyModel::filterAcceptsRowItself(int source_row, const QModelIndex &source_parent) const
{
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool TreeProxyModel::hasAcceptedChildren(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex item = sourceModel()->index(source_row,0,source_parent);
    if (!item.isValid()) {
        //qDebug() << "item invalid" << source_parent << source_row;
        return false;
    }

    //check if there are children
    int childCount = item.model()->rowCount(item);
    if (childCount == 0)
        return false;

    for (int i = 0; i < childCount; ++i) {
        if (filterAcceptsRowItself(i, item))
            return true;
        //recursive call -> NOTICE that this is depth-first searching, you're probably better off with breadth first search...
        if (hasAcceptedChildren(i, item))
            return true;
    }

    return false;
}

AtcCollectionEditorWidget::AtcCollectionEditorWidget(QWidget *parent) :
    QWidget(parent),
    d(new AtcCollectionEditorWidgetPrivate(this))
{
    d->ui = new Ui::AtcCollectionEditorWidget;
    // Creating UI
    d->ui->setupUi(this);
    d->ui->atcView->setIndentation(7);
    d->ui->atcView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->_left = new QToolButton(d->ui->search);
    d->aSearchEnglish = new QAction(this);
    d->aSearchEnglish->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
    d->aSearchCode = new QAction(this);
    d->aSearchCode->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
    d->_left->addAction(d->aSearchEnglish);
    d->_left->addAction(d->aSearchCode);
    d->_left->setDefaultAction(d->aSearchEnglish);
    d->ui->search->setLeftButton(d->_left);

    // Managing proxy models
    d->_proxyModel = new TreeProxyModel(this);
    d->_atcTreeProxyModel = new AtcTreeProxyModel(this);
    d->_atcTreeProxyModel->initialize(dbCore()->atcTableModel());
    d->ui->editor->setAtcTableModel(dbCore()->atcTableModel());
    d->_proxyModel->setSourceModel(d->_atcTreeProxyModel);
    d->ui->atcView->setModel(d->_proxyModel);
    for(int i = 0; i < dbCore()->atcTableModel()->columnCount(); ++i)
        d->ui->atcView->setColumnHidden(i, true);
    d->ui->atcView->setColumnHidden(AtcTreeProxyModel::Code, false);
    d->ui->atcView->setColumnHidden(AtcTreeProxyModel::LabelEn, false);

    // Ui connections
    connect(d->ui->atcView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onAtcCodeSelectionChanged(QModelIndex,QModelIndex)));
    connect(d->ui->search, SIGNAL(textChanged(QString)), this, SLOT(onFilterChanged(QString)));

    retranslateUi();
    d->ui->splitter->setStretchFactor(0, 1);
    d->ui->splitter->setStretchFactor(1, 2);
}

AtcCollectionEditorWidget::~AtcCollectionEditorWidget()
{
    if (d)
        delete d;
    d = 0;
}

void AtcCollectionEditorWidget::onAtcCodeSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    QModelIndex treeModelIndex = d->_proxyModel->mapToSource(current);
    d->ui->editor->setCurrentIndex(d->_atcTreeProxyModel->toSourceIndex(treeModelIndex));
}

void AtcCollectionEditorWidget::onFilterChanged(const QString &filter)
{
    d->_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    if (d->_left->defaultAction() == d->aSearchCode)
        d->_proxyModel->setFilterKeyColumn(AtcTreeProxyModel::Code);
    else if (d->_left->defaultAction() == d->aSearchEnglish)
        d->_proxyModel->setFilterKeyColumn(AtcTreeProxyModel::LabelEn);
    d->_proxyModel->setFilterFixedString(filter);
    if (filter.count() > 4)
        d->ui->atcView->expandAll();
}

void AtcCollectionEditorWidget::retranslateUi()
{
    d->aSearchEnglish->setText(tr("Filter on the english labels"));
    d->aSearchCode->setText(tr("Filter on the ATC code"));
//    for(int i = 0; i < dbCore()->atcTableModel()->columnCount(); ++i)
//        d->ui->atcView->setColumnHidden(i, true);
//    d->ui->atcView->setColumnHidden(AtcTableModel::Code, false);
//    // TODO: find the current language and set the correct column not hidden
//    d->ui->atcView->setColumnHidden(AtcTableModel::LabelEn, false);
//    d->ui->atcView->header()->setResizeMode(AtcTableModel::Code, QHeaderView::ResizeToContents);
//    d->ui->atcView->header()->setResizeMode(AtcTableModel::LabelEn, QHeaderView::Stretch);
}

void AtcCollectionEditorWidget::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        retranslateUi();
    }
}
