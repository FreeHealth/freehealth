#include "templatescreationdialog.h"
#include "ui_templatescreationdialog.h"

#include <templatesplugin/templatesmodel.h>

#include <translationutils/constanttranslations.h>

#include <QDebug>

using namespace Templates;
using namespace Trans::ConstantTranslations;

TemplatesCreationDialog::TemplatesCreationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Internal::Ui::TemplatesCreationDialog)
{
    ui->setupUi(this);
    setWindowTitle(qApp->applicationName() + " - " + ui->label->text());
    ui->parentCategory->setViewContent(TemplatesView::CategoriesOnly);
    ui->parentCategory->setEditMode(TemplatesView::None);
    ui->parentCategory->expandAll();
    ui->parentCategory->setSelectionMode(QAbstractItemView::SingleSelection);
}

TemplatesCreationDialog::~TemplatesCreationDialog()
{
    delete ui;
}

void TemplatesCreationDialog::done(int r)
{
    if (r==QDialog::Accepted) {
        // Do not accept templates witout content
        if (m_Content.isEmpty()) {
            QDialog::done(QDialog::Rejected);
            return;
        }
        // model --> insert row in parent index
        TemplatesModel *model = new TemplatesModel(this);
        QModelIndex parent = ui->parentCategory->currentItem();
        int row = model->rowCount(parent);
        if (!model->insertRow(row, parent)) {
            return;
        }
        // model --> setDatas
        model->setData(model->index(row, Constants::Data_IsTemplate, parent), true);
        QString tmp = ui->nameLineEdit->text();
        if (tmp.isEmpty())
            tmp = tkTr(Trans::Constants::FILENEW_TEXT);
        model->setData(model->index(row, Constants::Data_Label, parent), tmp);
        model->setData(model->index(row, Constants::Data_Summary, parent), ui->summaryTextEdit->toHtml());
        model->setData(model->index(row, Constants::Data_Content, parent), m_Content);
        model->setData(model->index(row, Constants::Data_ContentMimeTypes, parent), m_Mimes);
        model->setData(model->index(row, Constants::Data_IsNewlyCreated, parent), true);
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
