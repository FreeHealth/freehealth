/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
 ***************************************************************************/
#include "formplaceholder.h"

#include <utils/widgets/minisplitter.h>

#include <QTreeView>
#include <QTreeWidgetItem>
#include <QStackedLayout>
#include <QScrollArea>
#include <QTableView>
#include <QHeaderView>
#include <QPainter>
#include <QEvent>
#include <QDebug>

using namespace Form;

namespace Form {
namespace Internal {
class FormPlaceHolderPrivate
{
public:
    FormPlaceHolderPrivate() : m_FileTree(0), m_EpisodesTable(0), m_Stack(0), m_GeneralLayout(0)
    {
    }

    ~FormPlaceHolderPrivate()
    {
        delete m_FileTree; m_FileTree = 0;
        delete m_Stack; m_Stack = 0;
        delete m_GeneralLayout; m_GeneralLayout=0;
    }

    QTreeView *m_FileTree;
    QTableView *m_EpisodesTable;
    QStackedLayout *m_Stack;
    QGridLayout *m_GeneralLayout;
    QScrollArea *m_Scroll;
};

FormTreeView::FormTreeView(QWidget *parent) : QTreeView(parent)
{}

FormTreeView::~FormTreeView()
{}

bool FormTreeView::viewportEvent(QEvent *event)
{
    if (event->type()==QEvent::Paint) {
        qWarning() << "jjjjjjjjjjjjjjjjjjjjj";
        QPainter p(this);
//        p.begin();

        QRect rect = viewport()->rect();

        QColor background = QColor(0, 0, 0, 10);
        QColor light = QColor(255, 255, 255, 40);
        QColor dark = QColor(0, 0, 0, 60);

        QLinearGradient gr(QPoint(rect.center().x(), 0), QPoint(rect.center().x(), rect.bottom()));
        gr.setColorAt(0, Qt::white);
        gr.setColorAt(0.3, QColor(250, 250, 250));
        gr.setColorAt(0.7, QColor(230, 230, 230));

        p.fillRect(rect, gr);
        p.setPen(QColor(200, 200, 200));
        p.drawLine(rect.topLeft(), rect.topRight());
        p.setPen(QColor(150, 160, 200));
        p.drawLine(rect.bottomLeft(), rect.bottomRight());
    }
    return true;
}

}
}

FormPlaceHolder::FormPlaceHolder(QWidget *parent) :
        QWidget(parent), d(new Internal::FormPlaceHolderPrivate)
{
    d->m_GeneralLayout = new QGridLayout(this);
    d->m_GeneralLayout->setObjectName("FormPlaceHolder::GeneralLayout");
    d->m_GeneralLayout->setMargin(0);
    d->m_GeneralLayout->setSpacing(0);
    setLayout(d->m_GeneralLayout);

    QWidget *w = new QWidget(this);
    d->m_FileTree = new Internal::FormTreeView(this);
    d->m_FileTree->setObjectName("FormTree");
    d->m_FileTree->setIndentation(10);
//    d->m_FileTree->setStyleSheet("QTreeView#FormTree{background:#dee4ea}");
    d->m_Scroll = new QScrollArea(this);
    d->m_Scroll->setWidgetResizable(true);

    d->m_Stack = new QStackedLayout(w);
    d->m_Stack->setObjectName("FormPlaceHolder::StackedLayout");

    Utils::MiniSplitter *horiz = new Utils::MiniSplitter(this);
    horiz->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    horiz->setObjectName("FormPlaceHolder::MiniSplitter1");
    horiz->setOrientation(Qt::Horizontal);

    Utils::MiniSplitter *vertic = new Utils::MiniSplitter(this);
    vertic->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vertic->setObjectName("FormPlaceHolder::MiniSplitter::Vertical");
    vertic->setOrientation(Qt::Vertical);

    d->m_EpisodesTable = new QTableView(this);
    d->m_EpisodesTable->horizontalHeader()->hide();
    d->m_EpisodesTable->verticalHeader()->hide();

    horiz->addWidget(d->m_FileTree);
//    vertic->addWidget(d->m_EpisodesTable);
    vertic->addWidget(d->m_Scroll);
    horiz->addWidget(vertic);

    horiz->setStretchFactor(0, 1);
    horiz->setStretchFactor(1, 3);
//    vertic->setStretchFactor(0, 1);
//    vertic->setStretchFactor(1, 3);

    d->m_Scroll->setWidget(w);

    d->m_GeneralLayout->addWidget(horiz, 100, 0);

    connect(d->m_FileTree, SIGNAL(activated(QModelIndex)), this, SLOT(changeStackedLayoutTo(QModelIndex)));
}

FormPlaceHolder::~FormPlaceHolder()
{
    if (d) {
        delete d;
        d = 0;
    }
}

QTreeView *FormPlaceHolder::formTree() const
{
    return d->m_FileTree;
}

QStackedLayout *FormPlaceHolder::formStackLayout() const
{
    return d->m_Stack;
}

void FormPlaceHolder::clearFormStackLayout()
{
    for(int i = d->m_Stack->count(); i>0; --i) {
        QWidget *w = d->m_Stack->widget(i);
        d->m_Stack->removeWidget(w);
        delete w;
    }
}

void FormPlaceHolder::addTopWidget(QWidget *top)
{
    static int lastInsertedRow = 0;
    d->m_GeneralLayout->addWidget(top, lastInsertedRow, 0);
    ++lastInsertedRow;
}

void FormPlaceHolder::addBottomWidget(QWidget *bottom)
{
    d->m_GeneralLayout->addWidget(bottom, d->m_GeneralLayout->rowCount(), 0, 0, d->m_GeneralLayout->columnCount());
}

void FormPlaceHolder::changeStackedLayoutTo(const QModelIndex &index)
{
//    int id = item->data(0,Qt::UserRole).toInt();
//    d->m_Stack->setCurrentIndex(id);
}
