#include "pmhcategorydialog.h"
#include "pmhcategorymodel.h"

#include "ui_pmhcategorywidget.h"

#include <QDebug>


using namespace PMH;
using namespace Internal;

namespace PMH {
namespace Internal {
class PmhCategoryDialogPrivate
{
public:
    PmhCategoryDialogPrivate() : m_Widget(0), ui(0), m_Model(0)
    {
    }

    ~PmhCategoryDialogPrivate() {}

public:
    QWidget *m_Widget;
    Ui::PmhCategoryWidget *ui;
    PmhCategoryModel *m_Model;
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
}

void PmhCategoryDialog::done(int r)
{
    QDialog::done(r);
}
