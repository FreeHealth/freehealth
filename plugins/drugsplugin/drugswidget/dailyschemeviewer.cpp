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
#include "dailyschemeviewer.h"
#include "ui_dailyschemeviewer.h"

#include <drugsbaseplugin/dailyschememodel.h>

#include <utils/widgets/spinboxdelegate.h>

#include <QWidget>
#include <QObject>
#include <QDebug>

namespace DrugsWidget {
namespace Internal {

class DailySchemeViewerPrivate : public QWidget
{
    Q_OBJECT
public:
    DailySchemeViewerPrivate(DailySchemeViewer *parent) :
            q(parent),
            m_ui(new Ui::DailySchemeViewer),
            m_SpinDelegate(0)
    {
        m_ui->setupUi(q);
        connect(m_ui->repeatRadio, SIGNAL(toggled(bool)), this, SLOT(on_repeatRadio_toggled(bool)));
        connect(m_ui->distribRadio,SIGNAL(toggled(bool)), this, SLOT(on_distribRadio_toggled(bool)));
    }

    ~DailySchemeViewerPrivate()
    {
        if (m_ui) {
            delete m_ui;
            m_ui = 0;
        }
        if (m_SpinDelegate) {
            delete m_SpinDelegate;
            m_SpinDelegate = 0;
        }
    }

public Q_SLOTS:
    void on_repeatRadio_toggled(bool checked)
    {
        if (checked) {
            m_ui->dailyTableView->setItemDelegateForColumn(DrugsDB::DailySchemeModel::Value, 0);
            m_ui->dailyTableView->hideColumn(1);
            m_ui->dailyTableView->resizeColumnToContents(0);
            q->model()->setMethod(DrugsDB::DailySchemeModel::Repeat);
        } else {
            q->model()->setMethod(DrugsDB::DailySchemeModel::Distribute);
            m_ui->dailyTableView->showColumn(1);
            m_ui->dailyTableView->resizeColumnToContents(0);
            m_ui->dailyTableView->setItemDelegateForColumn(DrugsDB::DailySchemeModel::Value, m_SpinDelegate);
        }
    }

    void on_distribRadio_toggled(bool checked)
    {
        on_repeatRadio_toggled(!checked);
    }

    void on_model_methodchanged()
    {
        if (q->model()->method() == DrugsDB::DailySchemeModel::Repeat) {
            m_ui->repeatRadio->toggle();
        } else {
            m_ui->distribRadio->toggle();
        }
    }

public:
    DailySchemeViewer *q;
    Ui::DailySchemeViewer *m_ui;
    Utils::SpinBoxDelegate *m_SpinDelegate;
};
}  //  end namespace Internal
}  // end namespace DrugsWidget





using namespace DrugsWidget::Internal;

DailySchemeViewer::DailySchemeViewer(QWidget *parent) :
        QWidget(parent),
        d(new DailySchemeViewerPrivate(this))
{
    d->m_SpinDelegate = new Utils::SpinBoxDelegate(this);
    d->m_SpinDelegate->setDouble(true);
    d->m_ui->dailyTableView->setItemDelegateForColumn(DrugsDB::DailySchemeModel::Value, d->m_SpinDelegate);
}

DailySchemeViewer::~DailySchemeViewer()
{
}

void DailySchemeViewer::setModel(DrugsDB::DailySchemeModel *model)
{
    d->m_ui->dailyTableView->setModel(model);
    d->m_ui->dailyTableView->resizeColumnToContents(0);
    connect(model,SIGNAL(methodChanged()), d, SLOT(on_model_methodchanged()));
    d->on_model_methodchanged();
}

DrugsDB::DailySchemeModel *DailySchemeViewer::model() const
{
    return static_cast<DrugsDB::DailySchemeModel *>(d->m_ui->dailyTableView->model());
}

void DailySchemeViewer::setScoredTablet(bool isScored)
{
//    d->m_IsDouble = isScored;
    d->m_SpinDelegate->setDouble(isScored);
}

void DailySchemeViewer::setDailyMaximum(double maximum)
{
    model()->setMaximumDay(maximum);
    d->m_SpinDelegate->setMaximum(maximum-model()->sum());
}

void DailySchemeViewer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

#include "dailyschemeviewer.moc"
