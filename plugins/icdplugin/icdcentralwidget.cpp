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
#include "icdcentralwidget.h"
#include "icdmodel.h"
#include "icdcollectionmodel.h"
#include "icddialog.h"
#include "icdassociation.h"
#include "icdwidgetmanager.h"

#include "ui_icdcentralwidget.h"

#include <QDebug>

using namespace ICD;

namespace ICD {
namespace Internal {

class IcdCentralWidgetPrivate
{
public:
    IcdCentralWidgetPrivate(IcdCentralWidget *parent) :
            m_IcdSearchModel(0), m_CollectionModel(0), q(parent) {}

public:
    IcdSearchModel *m_IcdSearchModel;
    IcdCollectionModel *m_CollectionModel;

private:
    IcdCentralWidget *q;
};
}  // End namespace Internal
}  // End namespace ICD



IcdCentralWidget::IcdCentralWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IcdCentralWidget),
    d(new Internal::IcdCentralWidgetPrivate(this))
{
    // Ensure that manager is instanciated
    IcdWidgetManager::instance();

    d->m_IcdSearchModel = new ICD::IcdSearchModel(this);
    ui->setupUi(this);
    ui->selector->setModel(d->m_IcdSearchModel);
    ui->selector->initialize();
    d->m_CollectionModel = new IcdCollectionModel(this);
    ui->collectionView->setModel(d->m_CollectionModel);
    connect(ui->selector, SIGNAL(activated(QVariant)), this, SLOT(TEST_icdDialog(QVariant)));
}

IcdCentralWidget::~IcdCentralWidget()
{
    delete ui;
    if (d)
        delete d;
    d = 0;
}

void IcdCentralWidget::TEST_icdDialog(const QVariant &SID)
{
    ICD::IcdDialog dlg(SID, this);
    if (dlg.exec()==QDialog::Accepted) {
        qWarning()
                << "isValid" << dlg.isSelectionValid()
                << "\nisUnique" << dlg.isUniqueCode()
                << "\nisAssociation" << dlg.isAssociation()
                << "\ngetSidCode" << dlg.getSidCode();
        foreach(const Internal::IcdAssociation &asso, dlg.getAssocation()) {
            qWarning() << "asso" << asso.mainCodeWithDagStar() << asso.associatedCodeWithDagStar()
                    << asso.isValid();
        }
    }

}

void IcdCentralWidget::changeEvent(QEvent *e)
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
