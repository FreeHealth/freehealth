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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DrugsDB::Internal::InteractorSelectorDialog
 * Dialog to select an interacting molecule or class already recorded in the models.
 */

#include "interactorselectordialog.h"
#include "drugdruginteractioncore.h"
#include "druginteractor.h"

#include <translationutils/constants.h>
#include <utils/global.h>

#include <QStringListModel>
#include <QSortFilterProxyModel>

#include <QDebug>

#include "ui_interactorselectorwidget.h"

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugDrugInteractionCore *ddiCore() {return DrugsDB::DrugDrugInteractionCore::instance();}

namespace DrugsDB {
namespace Internal {
class InteractorSelectorDialogPrivate
{
public:
    InteractorSelectorDialogPrivate(InteractorSelectorDialog *parent) :
        ui(0),
        _selectedNames(0),
        _classProxyModel(0),
        _molProxyModel(0),
        _allowMoleculeCreation(false),
        _allowMultipleSelection(false),
        q(parent)
    {
    }
    
    ~InteractorSelectorDialogPrivate()
    {
        delete ui;
    }
    
public:
    Ui::InteractorSelectorWidget *ui;
    QStringListModel *_selectedNames;
    QSortFilterProxyModel *_classProxyModel, *_molProxyModel;

    bool _allowMoleculeCreation, _allowMultipleSelection;

private:
    InteractorSelectorDialog *q;
};
}  // namespace Internal
} // end namespace DrugsDB

/*!
 * Constructor of the DrugsDB::Internal::InteractorSelectorDialog class. \n
 * By default, the dialog does not allow neither multiple selection, nor molecule creation.
 */
InteractorSelectorDialog::InteractorSelectorDialog(QWidget *parent) :
    QDialog(parent),
    d(new InteractorSelectorDialogPrivate(this))
{
    d->ui = new Ui::InteractorSelectorWidget;
    d->ui->setupUi(this);

    d->ui->label->setText(tr("Interator selector"));

    d->ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help | QDialogButtonBox::Reset);

    d->_selectedNames = new QStringListModel(this);
    d->ui->selectedList->setModel(d->_selectedNames);

    d->_classProxyModel = new QSortFilterProxyModel(this);
    d->_classProxyModel->setSourceModel(ddiCore()->interactingClassesModel());
    d->_classProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    d->_classProxyModel->setSortRole(Qt::DisplayRole);
    d->_classProxyModel->setFilterKeyColumn(DrugInteractorModel::TrLabel);
    d->_classProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    d->ui->classView->setModel(d->_classProxyModel);
    d->ui->classView->setModelColumn(DrugInteractorModel::TrLabel);

    d->_molProxyModel = new QSortFilterProxyModel(this);
    d->_molProxyModel->setSourceModel(ddiCore()->interactingMoleculesModel());
    d->_molProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    d->_molProxyModel->setSortRole(Qt::DisplayRole);
    d->_molProxyModel->setFilterKeyColumn(DrugInteractorModel::TrLabel);
    d->_molProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    d->ui->moleculeView->setModel(d->_molProxyModel);
    d->ui->moleculeView->setModelColumn(DrugInteractorModel::TrLabel);

    connect(d->ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
    connect(d->ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(d->ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(d->ui->classView, SIGNAL(activated(QModelIndex)), this, SLOT(onClassInteractorActivated(QModelIndex)));
    connect(d->ui->moleculeView, SIGNAL(activated(QModelIndex)), this, SLOT(onMoleculeInteractorActivated(QModelIndex)));

    connect(d->ui->classSearchLine, SIGNAL(textChanged(QString)), this, SLOT(onSearchTextchanged(QString)));

    // Managing defaults
    setAllowMoleculeInteractorCreation(false);
    setAllowMultipleSelection(false);
}

/*! Destructor of the DrugsDB::Internal::InteractorSelectorDialog class */
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
void InteractorSelectorDialog::setAllowMultipleSelection(bool allow)
{
    d->_allowMultipleSelection = allow;
    d->ui->selectedList->setVisible(allow);
}

/** Set the title inside of the dialog. */
void InteractorSelectorDialog::setTitle(const QString &title)
{
    d->ui->label->setText(title);
}

/** Return the selected interactors. All accents are removed */
QStringList InteractorSelectorDialog::selectedNames() const
{
    QStringList toReturn;
    if (!d->_allowMultipleSelection) {
        if (d->ui->classView->selectionModel()->hasSelection())
            toReturn << d->ui->classView->selectionModel()->currentIndex().data().toString();
        if (d->ui->moleculeView->selectionModel()->hasSelection())
            toReturn << d->ui->moleculeView->selectionModel()->currentIndex().data().toString();
    } else {
        toReturn = d->_selectedNames->stringList();
    }

    QStringList noAccent;
    foreach(const QString &r, toReturn) {
        noAccent << Utils::removeAccents(r);
    }
    return noAccent;
}

void InteractorSelectorDialog::onButtonClicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton but = d->ui->buttonBox->standardButton(button);
    switch (but) {
    case QDialogButtonBox::Help: break;
    case QDialogButtonBox::Reset: d->_selectedNames->removeRows(0, d->_selectedNames->rowCount()); break;
    default: break;
    }
}

void InteractorSelectorDialog::onClassInteractorActivated(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QModelIndex sourceIndex = d->_classProxyModel->mapToSource(index);
    const QString &name = sourceIndex.data().toString();
    if (!d->_selectedNames->stringList().contains(name)) {
        if (!d->_allowMultipleSelection)
            d->_selectedNames->removeRows(0, d->_selectedNames->rowCount());
        int row = d->_selectedNames->rowCount();
        d->_selectedNames->insertRow(row);
        d->_selectedNames->setData(d->_selectedNames->index(row), name);
    }
}

void InteractorSelectorDialog::onMoleculeInteractorActivated(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QModelIndex sourceIndex = d->_molProxyModel->mapToSource(index);
    const QString &name = sourceIndex.data().toString();
    if (!d->_selectedNames->stringList().contains(name)) {
        if (!d->_allowMultipleSelection)
            d->_selectedNames->removeRows(0, d->_selectedNames->rowCount());
        int row = d->_selectedNames->rowCount();
        d->_selectedNames->insertRow(row);
        d->_selectedNames->setData(d->_selectedNames->index(row), name);
    }
}

void InteractorSelectorDialog::onSearchTextchanged(const QString &text)
{
    if (text.contains("*")) {
        d->_classProxyModel->setFilterWildcard(text);
        d->_molProxyModel->setFilterWildcard(text);
    } else if (text.contains("%")) {
        QString w = text;
        w = w.replace("%", "*");
        d->_classProxyModel->setFilterWildcard(w);
        d->_molProxyModel->setFilterWildcard(w);
    } else {
        d->_classProxyModel->setFilterFixedString(text);
        d->_molProxyModel->setFilterFixedString(text);
    }
}
