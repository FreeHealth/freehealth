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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "templateseditdialog.h"
#include "templatesmodel.h"
#include "ui_templateseditdialog.h"
#include "ui_templatescontenteditor.h"
#include "constants.h"

#include <translationutils/constanttranslations.h>

#include <QPersistentModelIndex>
#include <QTreeView>
#include <QDataWidgetMapper>
#include <QGridLayout>
#include <QTextEdit>
#include <QDebug>

using namespace Templates;
using namespace Trans::ConstantTranslations;

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
        if (m_Mapper) {
            delete m_Mapper;
            m_Mapper = 0;
        }
    }

    void refreshComboCategory()
    {
        if (!m_Index)
            return;
        if (!m_Model)
            return;
        if (!m_ui->parentTreeView->model()) {
            Templates::TemplatesModel *model = new Templates::TemplatesModel(q);
            model->categoriesOnly();
            model->setReadOnly(true);
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
            m_Mapper->addMapping(m_ui->userLineEdit, Constants::Data_UserUuid);
            m_Mapper->addMapping(m_ui->nameLineEdit, Constants::Data_Label);
            m_Mapper->addMapping(m_ui->summaryTextEdit, Constants::Data_Summary, "html");
        }
    }

    void refreshContent()
    {
        QString content = m_Model->index(m_Index->row(), Constants::Data_Content).data().toString();
        m_ui->viewButton->setEnabled(content.isEmpty());
        m_ui->viewButton->setEnabled(m_Model->isTemplate(*m_Index));
        QModelIndex idx = m_Index->parent();
        m_ui->parentTreeView->setCurrentIndex(idx);
        // FIXME: scrollTo is buggy...
        m_ui->parentTreeView->scrollTo(idx);
    }

public:
    Templates::TemplatesEditDialog *q;
    Ui::TemplatesEditDialog *m_ui;
    Templates::TemplatesModel *m_Model;
    QPersistentModelIndex *m_Index;
    QDataWidgetMapper *m_Mapper;
};

class TemplatesContentEditor : public QDialog, private Ui::TemplatesContentEditor
{
public:
    TemplatesContentEditor(QWidget *parent) : QDialog(parent), Ui::TemplatesContentEditor()
    {
        setupUi(this);
    }

    void setContent(const QString &content)
    {
        this->contentTextEdit->setPlainText(content);
    }

    void done(int r)
    {
        QDialog::done(r);
    }
};

}  // End Internal
}  // End Templates


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
        if (d->m_ui->nameLineEdit->text().isEmpty()) {
            d->m_ui->nameLineEdit->setFocus();
            d->m_ui->nameLineEdit->setText(tkTr(Trans::Constants::FILENEW_TEXT).remove("&"));
        }
        // modify focus in order to the mapper to get the changes done
        d->m_ui->buttonBox->setFocus();
        // submit mapper to model
        d->m_Mapper->submit();
        // reparent item
        QModelIndex idx = d->m_ui->parentTreeView->currentIndex();
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
    d->m_Index = new QPersistentModelIndex(d->m_Model->index(index.row(), Constants::Data_Label, index.parent()));
    d->refreshComboCategory();
    d->createMapper();
    d->m_Mapper->setRootIndex(d->m_Index->parent());
    d->m_Mapper->setCurrentIndex(d->m_Index->row());
    d->refreshContent();
}

void TemplatesEditDialog::editContent()
{
    Internal::TemplatesContentEditor dlg(this);
    dlg.setContent(d->m_Model->index(d->m_Index->row(), Constants::Data_Content, d->m_Index->parent()).data().toString());
    dlg.exec();
    // TODO: Manage changes done to the content
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
