/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "icdviewer.h"
#include "simpleicdmodel.h"
#include "fullicdcodemodel.h"

#include "ui_icdviewer.h"

#include <QStringListModel>

using namespace ICD;


namespace ICD {
namespace Internal {

class IcdViewerPrivate
{
public:
    IcdViewerPrivate() : ui(0), m_Model(0) {}

public:
    Ui::IcdViewer *ui;
    FullIcdCodeModel *m_Model;
};

}  // End Internal
}  // End ICD

IcdViewer::IcdViewer(QWidget *parent) :
    QWidget(parent),
    d(new Internal::IcdViewerPrivate)
{
    d->ui = new Ui::IcdViewer;
    d->ui->setupUi(this);
    d->ui->note->hide();
}

IcdViewer::~IcdViewer()
{
    if (d)
        delete d;
    d = 0;
}

void IcdViewer::setCodeSid(const QVariant &sid)
{
    d->m_Model = new FullIcdCodeModel(this);
    d->m_Model->setCode(sid.toInt());

    d->ui->codeLabel->setText(d->m_Model->index(0, FullIcdCodeModel::ICD_CodeWithDagetAndStar).data().toString());

    d->ui->labelsCombo->setModel(d->m_Model->labelsModel());
    d->ui->labelsCombo->setModelColumn(0);

    d->ui->codeTreeView->setModel(d->m_Model->codeTreeModel());
    d->ui->codeTreeView->expandAll();
    d->ui->codeTreeView->header()->hide();

    if (d->m_Model->includedLabelsModel()->rowCount() > 0) {
        d->ui->includes->setModel(d->m_Model->includedLabelsModel());
        d->ui->includes->horizontalHeader()->setStretchLastSection(true);
        d->ui->includes->horizontalHeader()->hide();
        d->ui->includes->verticalHeader()->hide();
        d->ui->includeGr->show();
    } else {
        d->ui->includeGr->hide();
    }

    if (d->m_Model->excludedModel()->rowCount() > 0) {
        d->ui->excludes->setModel(d->m_Model->excludedModel());
        d->ui->excludes->horizontalHeader()->hideSection(FullIcdCodeModel::ICD_Code);
        d->ui->excludes->horizontalHeader()->hideSection(FullIcdCodeModel::SID_Code);
        d->ui->excludes->horizontalHeader()->hideSection(FullIcdCodeModel::Daget);
        d->ui->excludes->horizontalHeader()->hideSection(FullIcdCodeModel::Type);
        d->ui->excludes->horizontalHeader()->setStretchLastSection(true);
        d->ui->excludes->horizontalHeader()->hide();
        d->ui->excludes->verticalHeader()->hide();
        d->ui->excludeGr->show();
    } else {
        d->ui->excludeGr->hide();
    }

    if (d->m_Model->dagStarModel()->rowCount() > 0) {
        d->ui->dependencies->setModel(d->m_Model->dagStarModel());
        d->ui->dependencies->horizontalHeader()->hideSection(FullIcdCodeModel::ICD_Code);
        d->ui->dependencies->horizontalHeader()->hideSection(FullIcdCodeModel::SID_Code);
        d->ui->dependencies->horizontalHeader()->hideSection(FullIcdCodeModel::Daget);
        d->ui->dependencies->horizontalHeader()->hideSection(FullIcdCodeModel::Type);
        d->ui->dependencies->horizontalHeader()->setStretchLastSection(true);
        d->ui->dependencies->horizontalHeader()->hide();
        d->ui->dependencies->verticalHeader()->hide();
        d->ui->dependenciesGr->show();
    } else {
        d->ui->dependenciesGr->hide();
    }

    const QString &note = d->m_Model->index(0, FullIcdCodeModel::Memo).data().toString();
    if (note.isEmpty()) {
        d->ui->note->hide();
    } else {
        d->ui->note->show();
        d->ui->note->setText(note);
    }
}

FullIcdCodeModel *IcdViewer::icdModel() const
{
    return d->m_Model;
}

void IcdViewer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->ui->retranslateUi(this);
        d->ui->codeLabel->setText(d->m_Model->index(0, FullIcdCodeModel::ICD_CodeWithDagetAndStar).data().toString());
        d->ui->codeTreeView->expandAll();
        d->ui->labelsCombo->setCurrentIndex(0);
        break;
    default:
        break;
    }
}
