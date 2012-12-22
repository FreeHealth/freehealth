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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "categorydialog.h"
#include "categoryitem.h"
#include "icategorymodelhelper.h"
#include "categorylabelsmodel.h"
#include "categoryonlyproxymodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <listviewplugin/languagecombobox.h>
#include <listviewplugin/languagecomboboxdelegate.h>

#include <utils/log.h>

#include "ui_categorydialog.h"

#include <QGridLayout>
#include <QDebug>

using namespace Category;
using namespace Internal;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

namespace Category {
namespace Internal {
class CategoryDialogPrivate
{
public:
    CategoryDialogPrivate() : ui(0), m_Model(0), m_CategoryModel(0), m_CatLabelsModel(0)
    {
    }

    ~CategoryDialogPrivate()
    {
        if (m_CatLabelsModel)
            delete m_CatLabelsModel;
        m_CatLabelsModel = 0;
    }

    void populateCategoryWithUi()
    {
        CategoryItem *cat = 0;
        if (!m_CatLabelsModel) {
//            LOG_ERROR_FOR("CategoryDialog", "No valid model");
            return;
        }
        m_CatLabelsModel->submit();
        cat = m_CatLabelsModel->categoryItem();
        cat->setData(CategoryItem::ThemedIcon, ui->themedIcon->text());
//        cat->setData(CategoryItem::Password, ui->password->text());
        m_Model->updateCategory(cat);
    }

    void populateUiWithCategory()
    {
        CategoryItem *cat = 0;
        if (!m_CatLabelsModel) {
//            LOG_ERROR_FOR("CategoryDialog", "No valid model");
            return;
        }
        cat = m_CatLabelsModel->categoryItem();
        ui->tableView->setModel(m_CatLabelsModel);
        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->themedIcon->setText(cat->iconName());
    }

public:
    Ui::CategoryDialog *ui;
    ICategoryModelHelper *m_Model;
    CategoryOnlyProxyModel *m_CategoryModel; // Do not delete it
    CategoryLabelsModel *m_CatLabelsModel;
};

}
}

CategoryDialog::CategoryDialog(QWidget *parent) :
        QDialog(parent), d(new Internal::CategoryDialogPrivate)
{
    d->ui = new Ui::CategoryDialog;
    d->ui->setupUi(this);
    setWindowTitle(tr("Category manager"));
    setWindowIcon(theme()->icon(Core::Constants::ICONCATEGORY_MANAGER));
    d->ui->treeView->header()->hide();
    d->ui->treeView->header()->setStretchLastSection(true);
    connect(d->ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(d->ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void CategoryDialog::setCategoryModel(ICategoryModelHelper *model, const int labelColumn)
{
    Q_ASSERT(model);
    d->m_Model = model;
    d->m_CategoryModel = model->categoryOnlyModel();
    d->ui->treeView->setModel(d->m_CategoryModel);
    for(int i = 0; i < d->m_CategoryModel->columnCount(QModelIndex()); ++i) {
        d->ui->treeView->hideColumn(i);
    }
    d->ui->treeView->showColumn(labelColumn);
    d->ui->treeView->header()->setStretchLastSection(false);
#if QT_VERSION < 0x050000
    d->ui->treeView->header()->setResizeMode(labelColumn, QHeaderView::Stretch);
#else
    // Qt5
    d->ui->treeView->header()->setSectionResizeMode(labelColumn, QHeaderView::Stretch);
#endif
    connect(d->ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(editItem(QModelIndex, QModelIndex)));
    d->ui->treeView->setCurrentIndex(d->m_CategoryModel->index(0,0));
    d->ui->treeView->expandAll();
}

void CategoryDialog::editItem(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    // autosave
    d->populateCategoryWithUi();

    // get the categoryData pointer from the pmhCategoryModel
    QModelIndex sourceItem = d->m_CategoryModel->mapToSource(current);
    CategoryItem *cat = d->m_Model->categoryForIndex(sourceItem);
    if (!cat)
        return;

    // populate the labels views with the category labels
    if (!d->m_CatLabelsModel) {
        d->m_CatLabelsModel = new CategoryLabelsModel(this);
        Category::ICategoryModelHelper *categoryModel = qobject_cast<Category::ICategoryModelHelper *>(d->m_CategoryModel->sourceModel());
        Q_ASSERT(categoryModel);
        if (categoryModel) {
            connect(d->m_CatLabelsModel, SIGNAL(labelChanged(const Category::CategoryItem*)),
                    categoryModel, SLOT(updateCategoryLabel(const Category::CategoryItem*)));
        }
    }
    d->m_CatLabelsModel->setCategoryItem(cat);
    d->populateUiWithCategory();
    d->ui->tableView->tableView()->setItemDelegateForColumn(CategoryLabelsModel::Lang, new Views::LanguageComboBoxDelegate(this));
}

void CategoryDialog::done(int r)
{
    if (r==QDialog::Accepted) {
        if (d->m_CatLabelsModel) {
            d->m_CatLabelsModel->submit();
            CategoryItem *cat = d->m_CatLabelsModel->categoryItem();
            d->m_Model->updateCategory(cat);
        }
    }
    QDialog::done(r);
}
