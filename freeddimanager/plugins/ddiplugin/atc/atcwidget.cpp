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
 * \class DDI::AtcWidget
 * ATC classification visualisation page.
 */

#include "atcwidget.h"
#include "atctablemodel.h"
//#include "atctreemodel.h"
#include <ddiplugin/ddicore.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/widgets/detailswidget.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>

#include "ui_atcwidget.h"

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

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////   AtcWidget   /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
AtcWidget::AtcWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AtcWidget)
{
    // Creating UI
    ui->setupUi(this);
    ui->atcView->setIndentation(7);
    ui->atcView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _left = new QToolButton(ui->search);
    aSearchEnglish = new QAction(this);
    aSearchEnglish->setText(tr("Filter on the english labels"));
    aSearchEnglish->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
    aSearchCode = new QAction(this);
    aSearchCode->setText(tr("Filter on the ATC code"));
    aSearchCode->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
    _left->addAction(aSearchEnglish);
    _left->addAction(aSearchCode);
    _left->setDefaultAction(aSearchEnglish);
    ui->search->setLeftButton(_left);

    // Managing proxy model
    _proxyModel = new TreeProxyModel(this);
    _proxyModel->setSourceModel(dbCore()->atcTableModel());
    ui->atcView->setModel(_proxyModel);

    // Ui connections
    connect(ui->atcView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onAtcCodeSelectionChanged(QModelIndex,QModelIndex)));
    connect(ui->search, SIGNAL(textChanged(QString)), this, SLOT(onFilterChanged(QString)));

    // Create the ATC details widget & content
    _details = new Utils::DetailsWidget(this);
    QLabel *codeLabel = new QLabel(tr("Code"), _details);
    QLabel *englishLabel = new QLabel(tr("English"), _details);
    QLabel *frenchLabel = new QLabel(tr("French"), _details);
    QLabel *deutschLabel = new QLabel(tr("Deutsch"), _details);
    QLabel *spanishLabel = new QLabel(tr("Spanish"), _details);
    _code = new QLineEdit(_details);
    _english = new QLineEdit(_details);
    _french = new QLineEdit(_details);
    _deutsch = new QLineEdit(_details);
    _spanish = new QLineEdit(_details);
    _code->setReadOnly(true);
    _english->setReadOnly(true);
    _french->setReadOnly(true);
    _deutsch->setReadOnly(true);
    _spanish->setReadOnly(true);
    QWidget *container = new QWidget(_details);
    QFormLayout *form = new QFormLayout(container);
    form->setMargin(0);
    form->addRow(codeLabel, _code);
    form->addRow(englishLabel, _english);
    form->addRow(frenchLabel, _french);
    form->addRow(deutschLabel, _deutsch);
    form->addRow(spanishLabel, _spanish);
    _details->setWidget(container);
    _details->setSummaryText(tr("No ATC selected"));

    layout()->addWidget(_details);
}

AtcWidget::~AtcWidget()
{
    delete ui;
}

void AtcWidget::onAtcCodeSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    QModelIndex source = _proxyModel->mapToSource(current);
    AtcTableModel *model = dbCore()->atcTableModel();
    QModelIndex code = model->index(source.row(), AtcTableModel::Code, source.parent());
    QModelIndex en = model->index(source.row(), AtcTableModel::LabelEn, source.parent());
    QModelIndex fr = model->index(source.row(), AtcTableModel::LabelFr, source.parent());
    QModelIndex de = model->index(source.row(), AtcTableModel::LabelDe, source.parent());
    _details->setSummaryText(code.data().toString() + " - " + en.data().toString());
//    QModelIndex es = model->index(current.row(), AtcTableModel::ATC_SpanishLabel, current.parent());
    _code->setText(code.data().toString());
    _english->setText(en.data().toString());
    _french->setText(fr.data().toString());
    _deutsch->setText(de.data().toString());
//    _spanish->setText(es.data().toString());
}

void AtcWidget::onFilterChanged(const QString &filter)
{
    _proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    if (_left->defaultAction() == aSearchCode)
        _proxyModel->setFilterKeyColumn(AtcTableModel::Code);
    else if (_left->defaultAction() == aSearchEnglish)
        _proxyModel->setFilterKeyColumn(AtcTableModel::LabelEn);
    _proxyModel->setFilterFixedString(filter);
    if (filter.count() > 4)
        ui->atcView->expandAll();
}

void AtcWidget::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        for(int i = 0; i < dbCore()->atcTableModel()->columnCount(); ++i)
            ui->atcView->setColumnHidden(i, true);
        ui->atcView->setColumnHidden(AtcTableModel::Code, false);
        // TODO: find the current language and set the correct column not hidden
        ui->atcView->setColumnHidden(AtcTableModel::LabelEn, false);
        ui->atcView->header()->setResizeMode(AtcTableModel::Code, QHeaderView::ResizeToContents);
        ui->atcView->header()->setResizeMode(AtcTableModel::LabelEn, QHeaderView::Stretch);
    }
}
