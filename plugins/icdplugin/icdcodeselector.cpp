/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "icdcodeselector.h"
#include "icdmodel.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/constants_icons.h>

#include <QToolButton>
#include <QDebug>

#include "ui_icdcodeselector.h"

using namespace ICD;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }


IcdCodeSelector::IcdCodeSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IcdCodeSelector),
    m_SearchToolButton(0)
{
    ui->setupUi(this);
    m_SearchToolButton = new QToolButton;   // parent object will be redefined
    m_SearchToolButton->setPopupMode(QToolButton::InstantPopup);
    m_SearchToolButton->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
    ui->lineEdit->setLeftButton(m_SearchToolButton);
    ui->lineEdit->setDelayedSignals(true);
}

IcdCodeSelector::~IcdCodeSelector()
{
    delete ui;
}

void IcdCodeSelector::initialize()
{
    populateToolButtons();
}

void IcdCodeSelector::setModel(IcdModel *model)
{
    ui->tableView->setModel(model);
    ui->tableView->setColumnHidden(IcdModel::SID_Code, true);
    ui->tableView->setColumnHidden(IcdModel::ICD_Code, true);
    ui->tableView->setColumnHidden(IcdModel::Daget, true);
    ui->tableView->setColumnHidden(IcdModel::Type, true);
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(setFilter(QString)));
}

IcdModel *IcdCodeSelector::model() const
{
    return qobject_cast<IcdModel *>(ui->tableView->model());
}

void IcdCodeSelector::populateToolButtons()
{
    Core::Command *cmd = actionManager()->command(Constants::A_SEARCH_LABEL);
    m_SearchToolButton->addAction(cmd->action());
    cmd->action()->trigger();
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(setSearchByLabel()));
    cmd = actionManager()->command(Constants::A_SEARCH_CODE);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(setSearchByCode()));
    m_SearchToolButton->addAction(cmd->action());
}

void IcdCodeSelector::setSearchByLabel()
{
    if (model())
        model()->setSearchMethod(IcdModel::SearchByLabel);
}

void IcdCodeSelector::setSearchByCode()
{
    if (model())
        model()->setSearchMethod(IcdModel::SearchByCode);
}

void IcdCodeSelector::setFilter(const QString &search)
{
    if (model()) {
        model()->setFilter(search);
        ui->tableView->scrollToTop();
    }
}

void IcdCodeSelector::changeEvent(QEvent *e)
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
