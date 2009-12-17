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
#include "templatesview.h"
#include "ui_templatesview.h"
#include "templatesmodel.h"
#include "itemplates.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/constants.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <QObject>
#include <QWidget>
#include <QDebug>

using namespace Templates;
using namespace Templates::Internal;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }

namespace Templates {
namespace Internal {
class TemplatesViewPrivate : public QObject
{
    Q_OBJECT
public:
    TemplatesViewPrivate(TemplatesView *parent) : QObject(parent), q(parent), m_Model(0), m_ui(0)
    {
        m_ui = new Ui::TemplatesView;
        m_ui->setupUi(q);
        m_Model = new TemplatesModel(this);
        m_ui->categoryTreeView->setModel(m_Model);
        int i;
        for(i=1; i < m_ui->categoryTreeView->model()->columnCount(); ++i) {
            m_ui->categoryTreeView->setColumnHidden(i, true);
        }
        QList<int> sizes;
        foreach(const QVariant &s, settings()->value(Constants::S_SPLITTER_SIZES).toList()) {
            sizes << s.toInt();
        }
        if (sizes.count() == 0) {
            sizes << 1 << 1;
        }
        m_ui->splitter->setSizes(sizes);

        // make connections
        connect(m_ui->categoryTreeView->selectionModel(),
                SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                this,
                SLOT(on_categoryTreeView_changed(const QModelIndex&,const QModelIndex&)));
        connect(m_ui->splitter, SIGNAL(splitterMoved(int,int)),
                this, SLOT(on_splitter_move()));
    }
    ~TemplatesViewPrivate() {}

public Q_SLOTS:
    void on_categoryTreeView_changed(const QModelIndex &current, const QModelIndex &)
    {
        Q_EMIT q->currentTemplateChanged();
        QModelIndex sum = m_Model->index(current.row(), TemplatesModel::Data_Summary, current.parent());
        m_ui->summaryTextEdit->setText(sum.data().toString());
    }
    void on_splitter_move()
    {
        QList<QVariant> sizes;
        foreach(int s, m_ui->splitter->sizes())
            sizes << s;
        settings()->setValue(Constants::S_SPLITTER_SIZES, sizes);
    }


public:
    TemplatesView *q;
    TemplatesModel *m_Model;
    Ui::TemplatesView *m_ui;
};
}
}

TemplatesView::TemplatesView(QWidget *parent) :
    QWidget(parent),
    d(new TemplatesViewPrivate(this))
{

//    m_ui->categoryTreeView->setColumnHidden(TemplatesModel::Data_Label, false);

//    summaryTextEdit
//            templatesTableView
//            categoryTreeView
//            searchLine
}

TemplatesView::~TemplatesView()
{
    delete d->m_ui;
    d->m_ui = 0;
    if (d) {
        delete d;
        d = 0;
    }
}

void TemplatesView::changeEvent(QEvent *e)
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

#include "templatesview.moc"
