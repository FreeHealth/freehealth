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
