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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "simplecategorycreator.h"
#include "simplecategorymodel.h"

#include "ui_simplecategorycreator.h"

using namespace Views;

SimpleCategoryCreator::SimpleCategoryCreator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimpleCategoryCreator),
    m_LabelCol(0), m_IconCol(1)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));}

SimpleCategoryCreator::~SimpleCategoryCreator()
{
    delete ui;
}

void SimpleCategoryCreator::done(int r)
{
    if (r == QDialog::Accepted) {
        if (!m_Model->insertRow(m_Model->rowCount(m_Parent), m_Parent))
            return;
        m_Model->setData(m_Model->index(m_Model->rowCount(m_Parent)-1, m_LabelCol, m_Parent), ui->categoryLabel->text());
        m_Model->setData(m_Model->index(m_Model->rowCount(m_Parent)-1, m_IconCol, m_Parent), m_ThemedIconFileName);
    }
    QDialog::done(r);
}

void SimpleCategoryCreator::setModel(QAbstractItemModel *model, const QModelIndex &parent, const int columnToShow)
{
    m_Model = model;
    if (parent.model() != m_Model)
        return;
    m_Parent = parent;
    ui->treeView->setModel(model);
    ui->treeView->setCurrentIndex(parent);
    for(int i = 0; i < m_Model->columnCount(m_Parent); ++i) {
        ui->treeView->hideColumn(i);
    }
    ui->treeView->setColumnHidden(columnToShow, false);
    ui->treeView->header()->hide();
}

void SimpleCategoryCreator::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
