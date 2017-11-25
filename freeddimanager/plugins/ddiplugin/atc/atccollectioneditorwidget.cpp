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
 *  along with this program.                                               *
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
#include <ddiplugin/constants.h>
#include <ddiplugin/database/ddidatabase.h>
#include <ddiplugin/atc/atctreeproxymodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>

#include "ui_atccollectioneditorwidget.h"

#include <QLabel>
#include <QFormLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QToolBar>

#include <QDebug>

// FIXME: create new ATC cause ATC model to be duplicated each time a new ATC is created
// FIXME: searching in codes does not correctly work

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DDI::DDICore *ddiCore() {return DDI::DDICore::instance();}

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
        aNewItem(0),
        aEditItem(0),
        aSaveItem(0),
        aRestoreItem(0),
        _toolBar(0),
        q(parent)
    {
    }

    ~AtcCollectionEditorWidgetPrivate()
    {
        delete ui;
    }

    void createActionsAndToolBar()
    {
        aSearchEnglish = new QAction(q);
        aSearchCode = new QAction(q);
        aNewItem = new QAction(q);
        aEditItem = new QAction(q);
        aSaveItem = new QAction(q);
        aRestoreItem = new QAction(q);

        aSearchEnglish->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
        aSearchCode->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
        aNewItem->setIcon(theme()->icon(Core::Constants::ICONADD));
        aEditItem->setIcon(theme()->icon(Core::Constants::ICONEDIT));
        aSaveItem->setIcon(theme()->icon(Core::Constants::ICONSAVE));
        aRestoreItem->setIcon(theme()->icon(Core::Constants::ICONPREVIOUS));

        aSaveItem->setEnabled(false);
        aRestoreItem->setEnabled(false);

        _toolBar = new QToolBar(q);
        _toolBar->setIconSize(QSize(32, 32));
        _toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        _toolBar->addAction(aNewItem);
        _toolBar->addSeparator();
        _toolBar->addAction(aEditItem);
        _toolBar->addSeparator();
        _toolBar->addAction(aSaveItem);
         _toolBar->addSeparator();
         _toolBar->addAction(aRestoreItem);
        ui->toolbarLayout->addWidget(_toolBar);

        _left = new QToolButton(ui->search);
        _left->addAction(aSearchEnglish);
        _left->addAction(aSearchCode);
        _left->setDefaultAction(aSearchEnglish);
        ui->search->setLeftButton(_left);
    }

    void createModelsAndPopulateViews()
    {
        ui->atcView->setIndentation(10);
        ui->atcView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        _atcTreeProxyModel = new AtcTreeProxyModel(q);
        _atcTreeProxyModel->initialize(ddiCore()->atcTableModel());
        _proxyModel = new TreeProxyModel(q);
        _proxyModel->setSourceModel(_atcTreeProxyModel);

        ui->editor->setAtcTableModel(ddiCore()->atcTableModel());
        ui->atcView->setModel(_proxyModel);

        for(int i = 0; i < ddiCore()->atcTableModel()->columnCount(); ++i)
            ui->atcView->setColumnHidden(i, true);
        ui->atcView->setColumnHidden(AtcTreeProxyModel::Code, false);
        ui->atcView->setColumnHidden(AtcTreeProxyModel::LabelEn, false);
    }

    void connectActionsAndUi()
    {
        QObject::connect(aNewItem, SIGNAL(triggered()), q, SLOT(onNewItemRequested()));
        QObject::connect(aEditItem, SIGNAL(triggered()), q, SLOT(enabledDataMapperEnabled()));
        QObject::connect(aSaveItem, SIGNAL(triggered()), q, SLOT(submitDataMapper()));
        QObject::connect(aRestoreItem, SIGNAL(triggered()), q, SLOT(restoreDataMapper()));

        QObject::connect(ui->search, SIGNAL(textChanged(QString)), q, SLOT(onFilterChanged(QString)));
        QObject::connect(ui->atcView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), q, SLOT(onAtcCodeSelectionChanged(QModelIndex,QModelIndex)));
    }

public:
    Ui::AtcCollectionEditorWidget *ui;
    TreeProxyModel *_proxyModel;
    AtcTreeProxyModel *_atcTreeProxyModel;
    QToolButton *_left;
    QAction *aSearchEnglish, *aSearchCode;
    QAction *aNewItem;
    QAction *aEditItem;
    QAction *aSaveItem;
    QAction *aRestoreItem;
    QToolBar *_toolBar;

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

    d->createActionsAndToolBar();
    d->createModelsAndPopulateViews();
    d->connectActionsAndUi();

    d->ui->editor->setEditorEnabled(false);
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

/** When an ATC code is selected, populate the data widget mapper */
void AtcCollectionEditorWidget::onAtcCodeSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    QModelIndex treeModelIndex = d->_proxyModel->mapToSource(current);
    d->ui->editor->setCurrentIndex(d->_atcTreeProxyModel->toSourceIndex(treeModelIndex));
}

/** Update the atctreemodelproxy filter */
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

void AtcCollectionEditorWidget::enabledDataMapperEnabled()
{
    qWarning() << d->ui->editor->isEditorEnabled();
    if (!d->ui->editor->isEditorEnabled()) {
        d->ui->editor->setEditorEnabled(true);
        d->aSaveItem->setEnabled(true);
        d->aRestoreItem->setEnabled(true);
    }
}

void AtcCollectionEditorWidget::submitDataMapper()
{
    d->aSaveItem->setEnabled(false);
    if (!d->ui->editor->submit()) {
        Utils::warningMessageBox(tr("Unable to submit modification to the model"),
                                 tr("Changes will not correctly saved. Please check the data and "
                                    "contact the development team."));
    } else {
        Utils::informativeMessageBox(tr("Modification saved"),
                                 tr("Changes are correctly saved."));
    }
    d->ui->editor->setEditorEnabled(false);
    d->aRestoreItem->setEnabled(false);
    d->aEditItem->setEnabled(true);
}

void AtcCollectionEditorWidget::restoreDataMapper()
{
    onAtcCodeSelectionChanged(d->ui->atcView->currentIndex(), QModelIndex());
}

/** Create a new ATC */
void AtcCollectionEditorWidget::onNewItemRequested()
{
    // Ask for the code
    QString code = Utils::askUser(tr("New ATC code"), tr("New ATC code:"));
    if (code.isEmpty())
        return;
    if (ddiCore()->atcTableModel()->isCodeExists(code)) {
        Utils::warningMessageBox(tr("New ATC code already exists"),
                                 tr("The code %1 already exists in the ATC database.").arg(code));
        return;
    }

    // Ask for the Uid
    QString uid = Utils::askUser(tr("New ATC Uid"), tr("New ATC uid:"));
    if (uid.isEmpty())
        return;
    uid = Constants::correctedUid(uid);
    if (ddiCore()->atcTableModel()->isUidExists(uid)) {
        Utils::warningMessageBox(tr("New ATC uid already exists"),
                                 tr("The uid %1 already exists in the ATC database.").arg(uid));
        return;
    }

    // Create a new ATC
    if (!ddiCore()->atcTableModel()->createAtcCode(code, uid))
        return;

    // Find the newly created ATC code
    d->ui->search->setText(code);
    d->aSearchCode->trigger();
    onFilterChanged(code);
}

void AtcCollectionEditorWidget::retranslateUi()
{
    d->aSearchEnglish->setText(tr("Filter on the english labels"));
    d->aSearchCode->setText(tr("Filter on the ATC code"));
    d->aNewItem->setText(tr("New ATC"));
    d->aEditItem->setText(tr("Edit ATC"));
    d->aSaveItem->setText(tr("Save ATC"));
    d->aRestoreItem->setText(tr("Restore ATC"));

    d->aSearchEnglish->setToolTip(d->aSearchEnglish->text());
    d->aSearchCode->setToolTip(d->aSearchCode->text());
    d->aNewItem->setToolTip(d->aNewItem->text());
    d->aEditItem->setToolTip(d->aEditItem->text());
    d->aSaveItem->setToolTip(d->aSaveItem->text());
    d->aRestoreItem->setToolTip(d->aRestoreItem->text());
}

void AtcCollectionEditorWidget::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        retranslateUi();
    }
}

#ifdef WITH_TESTS
#include <coreplugin/imainwindow.h>
#include <utils/randomizer.h>
#include <QTest>

static inline Core::IMainWindow *mainWindow()  { return Core::ICore::instance()->mainWindow(); }

//TODO: write tests

void AtcCollectionEditorWidget::test_runAllTests()
{
    test_views();
    test_actions();
    test_itemCreation();
    test_edition();
}

void AtcCollectionEditorWidget::test_views()
{
}

void AtcCollectionEditorWidget::test_editorEnabledState(bool state)
{
    Q_UNUSED(state);
}

void AtcCollectionEditorWidget::test_actions()
{
}

void AtcCollectionEditorWidget::test_itemCreation()
{
}

void AtcCollectionEditorWidget::test_edition()
{
}

#endif // WITH_TESTS
