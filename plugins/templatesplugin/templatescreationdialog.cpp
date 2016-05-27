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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
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
    ui->parentCategory->templatesModel()->setObjectName("TemplateCategoryParent");
    ui->parentCategory->templatesModel()->setReadOnly(true);
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
        model->setObjectName("TemplateCreatorSaver");
        QModelIndex parent = ui->parentCategory->currentItem();
        int row = model->rowCount(parent);
        if (!model->insertRow(row, parent)) {
            return;
        }
        // model --> setData
        model->setData(model->index(row, Constants::Data_IsTemplate, parent), true);
        QString tmp = ui->nameLineEdit->text();
        if (tmp.isEmpty())
            tmp = tkTr(Trans::Constants::FILENEW_TEXT).remove("&");
        model->setData(model->index(row, Constants::Data_Label, parent), tmp);
        model->setData(model->index(row, Constants::Data_Summary, parent), ui->summaryTextEdit->toHtml());
        model->setData(model->index(row, Constants::Data_Content, parent), m_Content);
        model->setData(model->index(row, Constants::Data_ContentMimeTypes, parent), m_Mimes);
        model->setData(model->index(row, Constants::Data_IsNewlyCreated, parent), true);
//        qWarning() << ui->userLineEdit->text();
        model->setData(model->index(row, Constants::Data_UserUuid), ui->userLineEdit->text());
//        qWarning() << ui->userLineEdit->text();
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

void TemplatesCreationDialog::setUserUuid(const QString &uuid)
{
    ui->userLineEdit->setText(uuid);
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
