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

#include <QTreeWidget>
#include <QStackedLayout>
#include <QScrollArea>

using namespace Form;

namespace Form {
namespace Internal {
class FormPlaceHolderPrivate
{
public:
    FormPlaceHolderPrivate() : m_Tree(0), m_Stack(0), m_GeneralLayout(0)
    {
    }

    ~FormPlaceHolderPrivate()
    {
        delete m_Tree; m_Tree = 0;
        delete m_Stack; m_Stack = 0;
        delete m_GeneralLayout; m_GeneralLayout=0;
    }

    QTreeWidget *m_Tree;
    QStackedLayout *m_Stack;
    QGridLayout *m_GeneralLayout;
    QScrollArea *m_Scroll;
};
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

    d->m_Tree = new QTreeWidget(this);
    d->m_Scroll = new QScrollArea(this);
    d->m_Scroll->setWidgetResizable(false);

    d->m_Stack = new QStackedLayout(d->m_Scroll);
    d->m_Stack->setObjectName("FormPlaceHolder::StackedLayout");
    d->m_Stack->setSizeConstraint(QLayout::SetFixedSize);
    d->m_Scroll->setLayout(d->m_Stack);

    Utils::MiniSplitter *s = new Utils::MiniSplitter(this);
    s->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    s->setObjectName("FormPlaceHolder::MiniSplitter1");
    s->setOrientation(Qt::Horizontal);
    s->addWidget(d->m_Tree);
    s->addWidget(d->m_Scroll);

    d->m_GeneralLayout->addWidget(s, 100, 0);

    connect(d->m_Tree, SIGNAL(itemActivated(QTreeWidgetItem*, int)),this,SLOT(changeStackedLayoutTo(QTreeWidgetItem*)));
}

FormPlaceHolder::~FormPlaceHolder()
{
    if (d) {
        delete d;
        d = 0;
    }
}

QTreeWidget *FormPlaceHolder::formTree() const
{
    return d->m_Tree;
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

void FormPlaceHolder::changeStackedLayoutTo(QTreeWidgetItem *item)
{
    int id = item->data(0,Qt::UserRole).toInt();
    d->m_Stack->setCurrentIndex(id);
}
