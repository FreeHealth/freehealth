#include "pmhcategorydialog.h"
#include "pmhcategorymodel.h"
#include "categorylabelsmodel.h"

#include "ui_pmhcategorywidget.h"

#include <QDebug>


using namespace PMH;
using namespace Internal;

namespace PMH {
namespace Internal {
class PmhCategoryDialogPrivate
{
public:
    PmhCategoryDialogPrivate() : m_Widget(0), ui(0), m_Model(0), m_CatLabelsModel(0)
    {
    }

    ~PmhCategoryDialogPrivate()
    {
        if (m_CatLabelsModel)
            delete m_CatLabelsModel;
        m_CatLabelsModel = 0;
    }

public:
    QWidget *m_Widget;
    Ui::PmhCategoryWidget *ui;
    PmhCategoryModel *m_Model;
    CategoryLabelsModel *m_CatLabelsModel;
};

}
}


PmhCategoryDialog::PmhCategoryDialog(QWidget *parent) :
        QDialog(parent), d(new Internal::PmhCategoryDialogPrivate)
{
    d->m_Widget = new QWidget(this);
    d->ui = new Ui::PmhCategoryWidget;
    d->ui->setupUi(d->m_Widget);
    d->ui->treeView->header()->hide();
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
    qWarning() << Q_FUNC_INFO;
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
}

void PmhCategoryDialog::done(int r)
{
    QDialog::done(r);
}
