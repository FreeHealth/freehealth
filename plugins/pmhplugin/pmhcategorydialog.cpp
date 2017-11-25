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
#include "pmhcategorydialog.h"
#include "pmhcategorymodel.h"
#include "categorylabelsmodel.h"

#include <listviewplugin/languagecombobox.h>
#include <listviewplugin/languagecomboboxdelegate.h>

#include "ui_pmhcategorywidget.h"

#include <QGridLayout>
#include <QDebug>


using namespace PMH;
using namespace Internal;

namespace PMH {
namespace Internal {
class PmhCategoryDialogPrivate
{
public:
    PmhCategoryDialogPrivate() : ui(0), m_Model(0), m_CatLabelsModel(0)
    {
    }

    ~PmhCategoryDialogPrivate()
    {
        if (m_CatLabelsModel)
            delete m_CatLabelsModel;
        m_CatLabelsModel = 0;
    }

public:
    Ui::PmhCategoryWidget *ui;
    PmhCategoryModel *m_Model;
    CategoryLabelsModel *m_CatLabelsModel;
};

}
}



PmhCategoryDialog::PmhCategoryDialog(QWidget *parent) :
        QDialog(parent), d(new Internal::PmhCategoryDialogPrivate)
{
    d->ui = new Ui::PmhCategoryWidget;
    d->ui->setupUi(this);
    d->ui->treeView->header()->hide();
    d->ui->treeView->header()->setStretchLastSection(true);
    connect(d->ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(d->ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void PmhCategoryDialog::setPmhCategoryModel(PmhCategoryModel *model)
{
    Q_ASSERT(model);
    d->m_Model = model;
    d->ui->treeView->setModel(model->categoryOnlyModel());
    d->ui->treeView->hideColumn(PmhCategoryModel::Id);
    d->ui->treeView->hideColumn(PmhCategoryModel::Type);
    d->ui->treeView->hideColumn(PmhCategoryModel::EmptyColumn);
    d->ui->treeView->header()->setStretchLastSection(false);
    d->ui->treeView->header()->setResizeMode(PmhCategoryModel::Label, QHeaderView::Stretch);
//    connect(d->ui->treeView, SIGNAL(entered(QModelIndex)), this, SLOT(editItem(QModelIndex)));
    connect(d->ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(editItem(QModelIndex, QModelIndex)));
}

void PmhCategoryDialog::editItem(const QModelIndex &current, const QModelIndex &)
{
    // get the categoryData pointer from the pmhCategoryModel
    QModelIndex sourceItem = d->m_Model->categoryOnlyModel()->mapToSource(current);
    PmhCategory *cat = d->m_Model->pmhCategoryforIndex(sourceItem);
    if (!cat)
        return;
    // populate the labels views with the category labels
    if (!d->m_CatLabelsModel) {
        d->m_CatLabelsModel = new CategoryLabelsModel(this);
    }
    d->m_CatLabelsModel->setPmhCategoryData(cat);
    d->ui->tableView->setModel(d->m_CatLabelsModel);
    d->ui->tableView->setItemDelegateForColumn(CategoryLabelsModel::Lang, new Views::LanguageComboBoxDelegate(this));
    d->ui->tableView->horizontalHeader()->setStretchLastSection(true);
}

void PmhCategoryDialog::done(int r)
{
    QDialog::done(r);
}
