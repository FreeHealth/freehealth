#include "templatescreationdialog.h"
#include "ui_templatescreationdialog.h"

#include <templatesplugin/templatesmodel.h>

#include <QDebug>

using namespace Templates;

TemplatesCreationDialog::TemplatesCreationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Internal::Ui::TemplatesCreationDialog)
{
    ui->setupUi(this);
    setWindowTitle(qApp->applicationName() + " - " + ui->label->text());
    ui->parentCategory->setViewContent(TemplatesView::CategoriesOnly);
    ui->parentCategory->setEditMode(TemplatesView::Add | TemplatesView::Edit);
}

TemplatesCreationDialog::~TemplatesCreationDialog()
{
    delete ui;
}

void TemplatesCreationDialog::done(int r)
{
    if (r==QDialog::Accepted) {
        // model --> insert row in parent index
        TemplatesModel *model = new TemplatesModel(this);
        QModelIndex parent = ui->parentCategory->currentItem();
        int row = model->rowCount(parent);
        if (!model->insertRow(row, parent)) {
            return;
        }
        // model --> setDatas
        model->setData(model->index(row, Constants::Data_IsTemplate, parent), true);
        model->setData(model->index(row, Constants::Data_Label, parent), ui->nameLineEdit->text());
        model->setData(model->index(row, Constants::Data_Summary, parent), ui->summaryTextEdit->toHtml());
        model->setData(model->index(row, Constants::Data_Content, parent), m_Content);
        model->setData(model->index(row, Constants::Data_ContentMimeTypes, parent), m_Mimes);
        delete model;
    }

    QDialog::done(r);
}

void TemplatesCreationDialog::setTemplateSummary(const QString &summary)
{
    ui->summaryTextEdit->setText(summary);
}

void TemplatesCreationDialog::setTemplateMimeTypes(const QStringList &list)
{
    m_Mimes = list;
}

void TemplatesCreationDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
