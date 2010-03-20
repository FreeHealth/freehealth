/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
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
        qWarning() << ui->userLineEdit->text();
        model->setData(model->index(row, Constants::Data_UserUuid), ui->userLineEdit->text());
        qWarning() << ui->userLineEdit->text();
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
