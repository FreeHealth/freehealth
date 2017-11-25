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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DDI::InteractorSelectorDialog
 * Dialog to select an interacting molecule or class already recorded in the models.
 */

#include "interactorselectordialog.h"
#include <ddiplugin/ddicore.h>
#include <ddiplugin/interactors/druginteractor.h>
#include <ddiplugin/interactors/druginteractortablemodel.h>
#include <ddiplugin/interactors/druginteractorsortfilterproxymodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/global.h>
#include <translationutils/constants.h>

#include <QStringListModel>
#include <QToolButton>

#include <QDebug>

#include "ui_interactorselectorwidget.h"

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DDI::DDICore *ddiCore() {return DDI::DDICore::instance();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

namespace DDI {
namespace Internal {
class InteractorSelectorDialogPrivate
{
public:
    InteractorSelectorDialogPrivate(InteractorSelectorDialog *parent) :
        ui(0),
        _selectedNames(0),
        _selectedUids(0),
        _molProxyModel(0),
        _allowMoleculeCreation(false),
        _allowMultipleSelection(0),
        aSearchAll(0), aSearchMols(0), aSearchClasses(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~InteractorSelectorDialogPrivate()
    {
        delete ui;
    }

public:
    Ui::InteractorSelectorWidget *ui;
    QStringListModel *_selectedNames, *_selectedUids;
    DrugInteractorSortFilterProxyModel *_molProxyModel;
    bool _allowMoleculeCreation;
    int _allowMultipleSelection;
    QAction *aSearchAll, *aSearchMols, *aSearchClasses;

private:
    InteractorSelectorDialog *q;
};
} // namespace Internal
} // namespace DrugsDB

/*!
 * Constructor of the DDI::InteractorSelectorDialog class. \n
 * By default, the dialog does not allow neither multiple selection, nor molecule creation.
 */
InteractorSelectorDialog::InteractorSelectorDialog(QWidget *parent) :
    QDialog(parent),
    d(new InteractorSelectorDialogPrivate(this))
{
    d->ui = new Ui::InteractorSelectorWidget;
    d->ui->setupUi(this);
    setWindowTitle(tr("Drug interactor selector"));

    // Prepare search tool button
    d->aSearchAll = new QAction(this);
    d->aSearchAll->setText(tr("Search an interactor: class or molecule name"));
    d->aSearchAll->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
    d->aSearchMols = new QAction(this);
    d->aSearchMols->setText(tr("Search an interactor: molecule name only"));
    d->aSearchMols->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
    d->aSearchClasses = new QAction(this);
    d->aSearchClasses->setText(tr("Search an interactor: class name only"));
    d->aSearchClasses->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
    QToolButton *left = new QToolButton(this);
    left->addAction(d->aSearchAll);
    left->addAction(d->aSearchMols);
    left->addAction(d->aSearchClasses);
    left->setDefaultAction(d->aSearchAll);
    d->ui->searchLine->setLeftButton(left);
    connect(left, SIGNAL(triggered(QAction*)), this, SLOT(onSearchActionChanged(QAction*)));

    // Some ui comestics
    d->ui->label->setText(tr("Interator selector"));
    d->ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help | QDialogButtonBox::Reset);
    d->_selectedNames = new QStringListModel(this);
    d->_selectedUids = new QStringListModel(this);
    d->ui->selectedList->setModel(d->_selectedNames);
    d->ui->selectedUids->setModel(d->_selectedUids);

    // Prepare drug interactors model & view
    d->_molProxyModel = new DrugInteractorSortFilterProxyModel(this);
    d->_molProxyModel->setSourceModel(ddiCore()->drugInteractorTableModel());
    d->_molProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    d->_molProxyModel->setSortRole(Qt::DisplayRole);
    d->_molProxyModel->sort(DrugInteractorTableModel::TranslatedLabel, Qt::AscendingOrder);
    d->_molProxyModel->setFilterKeyColumn(DrugInteractorTableModel::TranslatedLabel);
    d->_molProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    d->ui->moleculeView->setModel(d->_molProxyModel);
    d->ui->moleculeView->setModelColumn(DrugInteractorTableModel::TranslatedLabel);

    connect(d->ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
    connect(d->ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(d->ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(d->ui->moleculeView, SIGNAL(activated(QModelIndex)), this, SLOT(onMoleculeInteractorActivated(QModelIndex)));
    connect(d->ui->selectedList, SIGNAL(activated(QModelIndex)), this, SLOT(onSelectedListActivated(QModelIndex)));

    connect(d->ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(onSearchTextchanged(QString)));

    // Managing defaults
    setAllowMoleculeInteractorCreation(false);
    setAllowMultipleSelection(0);
}

/*! Destructor of the DDI::InteractorSelectorDialog class */
InteractorSelectorDialog::~InteractorSelectorDialog()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool InteractorSelectorDialog::initialize()
{
    return true;
}

/** Allow user to create molecule interactor inside the dialog. By default, set to false. */
void InteractorSelectorDialog::setAllowMoleculeInteractorCreation(bool allow)
{
    d->_allowMoleculeCreation = allow;
}

/** Allow user to multiple interactors. By default, set to false. */
void InteractorSelectorDialog::setAllowMultipleSelection(int maximumOfItemSelected)
{
    d->_allowMultipleSelection = maximumOfItemSelected;
    d->ui->selectedList->setVisible(maximumOfItemSelected>0);
}

/** Set the title inside of the dialog. */
void InteractorSelectorDialog::setTitle(const QString &title)
{
    d->ui->label->setText(title);
}

/** Return the selected interactors within a list of pairs. pair first is the name, second the uid. */
QList< QPair<QString, QString> > InteractorSelectorDialog::selectedInteractors() const
{
    QList< QPair<QString, QString> > toReturn;
    for(int i=0; i < d->_selectedNames->rowCount(); ++i) {
        QPair<QString, QString> pair;
        pair.first = d->_selectedNames->data(d->_selectedNames->index(i), Qt::DisplayRole).toString();
        pair.second = d->_selectedUids->data(d->_selectedUids->index(i), Qt::DisplayRole).toString();
        toReturn << pair;
    }
    return toReturn;
}

/** React on button clicked (help, remove) */
void InteractorSelectorDialog::onButtonClicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton but = d->ui->buttonBox->standardButton(button);
    switch (but) {
    case QDialogButtonBox::Help: break;
    case QDialogButtonBox::Reset: d->_selectedNames->removeRows(0, d->_selectedNames->rowCount()); break;
    default: break;
    }
}

/** Toggle the model filtering according to the trigerred search action  */
void InteractorSelectorDialog::onSearchActionChanged(QAction *a)
{
    if (a==d->aSearchAll)
        d->_molProxyModel->setClassMolFilter(DrugInteractorSortFilterProxyModel::ClassesAndMolecules);
    else if (a==d->aSearchMols)
        d->_molProxyModel->setClassMolFilter(DrugInteractorSortFilterProxyModel::MoleculesOnly);
    else if (a==d->aSearchClasses)
        d->_molProxyModel->setClassMolFilter(DrugInteractorSortFilterProxyModel::ClassesOnly);
    d->_molProxyModel->invalidate();
}

/** When a molecule is activated -> add it to the selected list */
void InteractorSelectorDialog::onMoleculeInteractorActivated(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    if (d->_allowMultipleSelection && d->_selectedNames->rowCount() >= d->_allowMultipleSelection)
        return;

    QModelIndex sourceIndex = d->_molProxyModel->mapToSource(index);
    QModelIndex uidIndex = ddiCore()->drugInteractorTableModel()->index(sourceIndex.row(), DrugInteractorTableModel::Uuid);
    const QString &name = sourceIndex.data().toString();
    const QString &uid = uidIndex.data().toString();
    if (!d->_selectedNames->stringList().contains(name)) {
        if (!d->_allowMultipleSelection) {
            d->_selectedNames->removeRows(0, d->_selectedNames->rowCount());
            d->_selectedUids->removeRows(0, d->_selectedNames->rowCount());
        }
        int row = d->_selectedNames->rowCount();
        d->_selectedNames->insertRow(row);
        d->_selectedNames->setData(d->_selectedNames->index(row), name);
        d->_selectedUids->insertRow(row);
        d->_selectedUids->setData(d->_selectedUids->index(row), uid);
    }
}

void InteractorSelectorDialog::onSelectedListActivated(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    int row = index.row();
    d->_selectedNames->removeRow(row);
    d->_selectedUids->removeRow(row);
}

void InteractorSelectorDialog::onSearchTextchanged(const QString &text)
{
    if (text.contains("*")) {
        d->_molProxyModel->setFilterWildcard(text);
    } else if (text.contains("%")) {
        QString w = text;
        w = w.replace("%", "*");
        d->_molProxyModel->setFilterWildcard(w);
    } else {
        d->_molProxyModel->setFilterFixedString(text);
    }
}

#ifdef WITH_TESTS
#include <QTimer>
void InteractorSelectorDialog::test_runTestAndAccept()
{
    QTimer::singleShot(10, this, SLOT(accept()));
}

#endif
