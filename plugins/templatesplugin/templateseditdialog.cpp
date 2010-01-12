/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
#include "templateseditdialog.h"
#include "templatesmodel.h"
#include "ui_templateseditdialog.h"
#include "ui_templatescontenteditor.h"
#include "constants.h"

#include <QPersistentModelIndex>
#include <QTreeView>
#include <QDataWidgetMapper>
#include <QGridLayout>
#include <QTextEdit>
#include <QDebug>

using namespace Templates;

namespace Templates {
namespace Internal {
class TemplatesEditDialogPrivate
{
public:
    TemplatesEditDialogPrivate(TemplatesEditDialog *parent) :
            q(parent),
            m_ui(new Ui::TemplatesEditDialog),
            m_Model(0),
            m_Index(0),
            m_Mapper(0)
    {
        m_ui->setupUi(q);
    }
    ~TemplatesEditDialogPrivate()
    {
        delete m_ui;
        if (m_Index) {
            delete m_Index;
            m_Index = 0;
        }
    }

    void refreshComboCategory()
    {
        if (!m_Index)
            return;
        if (!m_Model)
            return;
        if (!m_ui->parentTreeView->model()) {
            Templates::TemplatesModel *model = new Templates::TemplatesModel(m_ui->parentTreeView);
            model->categoriesOnly();
            m_ui->parentTreeView->setModel(model);
        }
        for(int i=0; i < m_Model->columnCount(); ++i) {
            m_ui->parentTreeView->setColumnHidden(i,true);
        }
        m_ui->parentTreeView->setColumnHidden(Constants::Data_Label, false);
        m_ui->parentTreeView->setIndentation(10);
        m_ui->parentTreeView->header()->hide();
        m_ui->parentTreeView->expandAll();
    }

    void createMapper()
    {
        if (!m_Model)
            return;
        if (!m_Index)
            return;
        if (!m_Mapper) {
            m_Mapper = new QDataWidgetMapper(q);
            m_Mapper->setModel(m_Model);
            m_Mapper->addMapping(m_ui->nameLineEdit, Constants::Data_Label);
            m_Mapper->addMapping(m_ui->summaryTextEdit, Constants::Data_Summary, "html");
        }
    }

    void refreshContent()
    {
        QString content = m_Model->index(m_Index->row(), Constants::Data_Content).data().toString();
        m_ui->viewButton->setEnabled(content.isEmpty());
        m_ui->viewButton->setEnabled(m_Model->isTemplate(*m_Index));
        // find parent category
        QModelIndex idx(*m_Index);
        while ((idx.isValid()) && (!m_Model->isCategory(idx))) {
            idx = idx.parent();
        }
        m_ui->parentTreeView->setCurrentIndex(idx);
    }

public:
    Templates::TemplatesEditDialog *q;
    Ui::TemplatesEditDialog *m_ui;
    Templates::TemplatesModel *m_Model;
    QPersistentModelIndex *m_Index;
    QDataWidgetMapper *m_Mapper;
};
}
}


TemplatesEditDialog::TemplatesEditDialog(QWidget *parent) :
        QDialog(parent), d(new Internal::TemplatesEditDialogPrivate(this))
{
    connect(d->m_ui->viewButton, SIGNAL(clicked()),this, SLOT(editContent()));
}

TemplatesEditDialog::~TemplatesEditDialog()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void TemplatesEditDialog::done(int r)
{
    if (r==QDialog::Rejected) {
        d->m_Mapper->revert();
    } else if (r==QDialog::Accepted) {
        // modify focus in order to the mapper to get the changes done
        d->m_ui->buttonBox->setFocus();
        // submit mapper to model
        d->m_Mapper->submit();
        // reparent item
        QModelIndex idx = d->m_ui->parentTreeView->selectionModel()->currentIndex();
        if (idx.isValid()) {
            d->m_Model->reparentIndex(*d->m_Index, idx);
        }
    }
    QDialog::done(r);
}

void TemplatesEditDialog::setModel(Templates::TemplatesModel *model)
{
    Q_ASSERT(model);
    if (!model)
        return;
    d->m_Model = model;
    d->createMapper();
}

void TemplatesEditDialog::setModelIndex(const QModelIndex &index)
{
    if (d->m_Index) {
        delete d->m_Index;
        d->m_Index = 0;
    }
    d->m_Index = new QPersistentModelIndex(index);
    d->refreshComboCategory();
    d->createMapper();
    d->m_Mapper->setRootIndex(index.parent());
    d->m_Mapper->setCurrentIndex(d->m_Index->row());
    d->refreshContent();
}

void TemplatesEditDialog::editContent()
{
    QDialog dlg(this);
    Internal::Ui::TemplatesContentEditor ui;
    ui.setupUi(&dlg);
    QString content = d->m_Model->index(d->m_Index->row(), Constants::Data_Content).data().toString();
    ui.contentTextEdit->setPlainText(content);
    dlg.exec();
}

void TemplatesEditDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
