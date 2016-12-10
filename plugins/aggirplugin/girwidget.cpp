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
 ***************************************************************************/
#include "girwidget.h"
#include "girmodel.h"
#include "ui_girwidget.h"

#include <formmanagerplugin/iformitem.h>
#include <medicalutils/aggir/girscore.h>

#include <QDebug>

using namespace Gir;
using namespace Gir::Internal;

GirWidgetFactory::GirWidgetFactory(QObject *parent) :
        IFormWidgetFactory(parent)
{
}

Form::IFormWidget *GirWidgetFactory::createWidget(const QString &name, Form::FormItem *object, QWidget *parent)
{
    if ((name.compare("aggir",Qt::CaseInsensitive)==0) ||
        (name.compare("gir",Qt::CaseInsensitive)==0)) {
        return new GirWidget(object, parent);
    }
    return 0;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------ GirUi -------------------------------------------------
//--------------------------------------------------------------------------------------------------------
GirUi::GirUi(QWidget *parent) :
    QWidget(parent),
    m_ui(0)
{
    m_ui = new Ui::GirWidget();
    m_ui->setupUi(this);

    // connect all QButtonGroup to Gir string preparation
    m_GirString.fill('_',13);
    m_Gir = 0;

    model = new GirModel(this);
    m_ui->treeView->setModel(model);
    m_ui->treeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_ui->treeView->setAlternatingRowColors(true);
    m_ui->treeView->header()->setStretchLastSection(false);
    m_ui->treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    for(int i=1; i < m_ui->treeView->model()->columnCount(); ++i)
        m_ui->treeView->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    m_ui->treeView->expandAll();
    m_ui->treeView->setMinimumHeight((m_ui->treeView->visualRect(model->index(0,0)).height()) * model->rowCountWithChildren() + 6);
    connect(model, SIGNAL(girCalculated(int)), this, SLOT(girCalculated(int)));
    connect(model, SIGNAL(modelReset()), m_ui->treeView, SLOT(expandAll()));
}

GirUi::~GirUi()
{
    if (m_ui)
        delete m_ui;
    m_ui=0;
}

void GirUi::girCalculated(const int gir)
{
    m_ui->girBar->setValue(gir);
    m_ui->girLineEdit->setText(QString::number(gir));
}

//static inline int calculateGirScore(const QString &s)
//{
//    MedicalUtils::AGGIR::OldGirScore score;
//    score.setValues(s[0], s[1], s.mid(2,2), s.mid(4,3), s.mid(7,2), s.mid(9,2),s[11], s[12]);
//    return score.resultingGir();
//}

void GirUi::setStringfiedGirScore(const QString &gir)
{
    model->setStringfiedGirScore(gir);
}

QString GirUi::stringfiedGirScore() const
{
    return model->stringfiedGirScore();
}

void GirUi::clearModel()
{
    model->clearGirScore();
}

QString GirUi::toHtml() const
{
    return model->toHtml(_html);
}

//--------------------------------------------------------------------------------------------------------
//---------------------------------------------- GirWidget -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
GirWidget::GirWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem, parent), m_ui(0)
{
    QBoxLayout * hb = getBoxLayout(OnTop, m_FormItem->spec()->label(), this);
    m_Label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    hb->addWidget(m_Label);
    m_ui = new GirUi(this);
    hb->addWidget(m_ui);
    QString filemask = formItem->spec()->value(Form::FormItemSpec::Spec_HtmlPrintMask).toString();
    if (!filemask.isEmpty())
        m_ui->setHtmlPrintMask(filemask);

    // create FormItemData
    GirItemData *data = new GirItemData(m_FormItem);
    data->setGirWidget(this);
    m_FormItem->setItemData(data);
}

GirWidget::~GirWidget()
{
}

void GirWidget::setStringfiedGirScore(const QString &gir)
{
    m_ui->setStringfiedGirScore(gir);
}

QString GirWidget::stringfiedGirScore() const
{
    return m_ui->stringfiedGirScore();
}

void GirWidget::clearModel()
{
    m_ui->clearModel();
}

QString GirWidget::printableHtml(bool withValues) const
{
    Q_UNUSED(withValues);
    // TODO: code here with values, print empty values
    return m_ui->toHtml();
}

//--------------------------------------------------------------------------------------------------------
//-------------------------------------------- GirItemData -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
GirItemData::GirItemData(Form::FormItem *parent) :
    Form::IFormItemData(),
    m_Parent(parent),
    m_GirWidget(0),
    m_modified(false)
{}

void GirItemData::clear()
{
    m_GirWidget->clearModel();
}

bool GirItemData::isModified() const
{
    return m_OriginalValue != storableData().toString();
}

void GirItemData::setModified(bool modified)
{
    m_modified = modified;
    if (!modified)
        m_OriginalValue = storableData().toString();
}

void GirItemData::setReadOnly(bool readOnly)
{
    m_readonly = readOnly;
    // TODO: improve this
    m_GirWidget->m_ui->setEnabled(!readOnly);
}

bool GirItemData::isReadOnly() const
{
    return m_readonly;
}

bool GirItemData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    Q_UNUSED(data);
    Q_UNUSED(role);
    return true;
}

QVariant GirItemData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    if (role == PrintRole) {
        return m_GirWidget->m_ui->toHtml();
    }
    return QVariant();
}

void GirItemData::setStorableData(const QVariant &data)
{
//    if (data.toString().compare(m_OriginalValue, Qt::CaseInsensitive)==0) {
//        // do nothing
//        return;
//    }
    m_OriginalValue = data.toString();
    m_GirWidget->setStringfiedGirScore(m_OriginalValue);
    m_modified = false;
}

QVariant GirItemData::storableData() const
{
    QString s = m_GirWidget->stringfiedGirScore();
    return s;
}


