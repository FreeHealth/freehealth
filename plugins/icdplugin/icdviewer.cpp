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
#include "icdviewer.h"
#include "icdmodel.h"
#include "fullicdcodemodel.h"

#include "ui_icdviewer.h"

#include <QStringListModel>

using namespace ICD;


IcdViewer::IcdViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IcdViewer)
{
    ui->setupUi(this);
    ui->note->hide();
}

IcdViewer::~IcdViewer()
{
    delete ui;
}

void IcdViewer::setCodeSid(const QVariant &sid)
{
    FullIcdCodeModel *model = new FullIcdCodeModel(this);
    model->setCode(sid.toInt());

    ui->codeLabel->setText(model->index(0, FullIcdCodeModel::ICD_CodeWithDagetAndStar).data().toString());

    ui->labelsCombo->setModel(model->labelsModel());
    ui->labelsCombo->setModelColumn(0);

    ui->codeTreeView->setModel(model->codeTreeModel());
    ui->codeTreeView->expandAll();
    ui->codeTreeView->header()->hide();

    if (model->includedLabelsModel()->rowCount() > 0) {
        ui->includes->setModel(model->includedLabelsModel());
        ui->includes->horizontalHeader()->setStretchLastSection(true);
        ui->includes->horizontalHeader()->hide();
        ui->includes->verticalHeader()->hide();
        ui->includeGr->show();
    } else {
        ui->includeGr->hide();
    }

    if (model->excludedModel()->rowCount() > 0) {
        ui->excludes->setModel(model->excludedModel());
        ui->excludes->horizontalHeader()->hideSection(FullIcdCodeModel::ICD_Code);
        ui->excludes->horizontalHeader()->hideSection(FullIcdCodeModel::SID_Code);
        ui->excludes->horizontalHeader()->hideSection(FullIcdCodeModel::Daget);
        ui->excludes->horizontalHeader()->hideSection(FullIcdCodeModel::Type);
        ui->excludes->horizontalHeader()->setStretchLastSection(true);
        ui->excludes->horizontalHeader()->hide();
        ui->excludes->verticalHeader()->hide();
        ui->excludeGr->show();
    } else {
        ui->excludeGr->hide();
    }

    if (model->dagStarModel()->rowCount() > 0) {
        ui->dependencies->setModel(model->dagStarModel());
        ui->dependencies->horizontalHeader()->hideSection(FullIcdCodeModel::ICD_Code);
        ui->dependencies->horizontalHeader()->hideSection(FullIcdCodeModel::SID_Code);
        ui->dependencies->horizontalHeader()->hideSection(FullIcdCodeModel::Daget);
        ui->dependencies->horizontalHeader()->hideSection(FullIcdCodeModel::Type);
        ui->dependencies->horizontalHeader()->setStretchLastSection(true);
        ui->dependencies->horizontalHeader()->hide();
        ui->dependencies->verticalHeader()->hide();
        ui->dependenciesGr->show();
    } else {
        ui->dependenciesGr->hide();
    }

    const QString &note = model->index(0, FullIcdCodeModel::Memo).data().toString();
    if (note.isEmpty()) {
        ui->note->hide();
    } else {
        ui->note->show();
        ui->note->setText(note);
    }
}

void IcdViewer::changeEvent(QEvent *e)
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
