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
 ***************************************************************************/

/**
  \class StringListView
  \brief Provides a stringlist view to use with tkUserViewer. For eg with the specialties, qualifications... of users.
  - Property stringList is used for QDataWidgetMapper.
  - Holds the mecanism for add, remove, move up and down for all items using a contextmenu.
  - Set and get the model stringList using getStringList() and setStringList().

  - You can use it with the tkStringListModel and set items to be checkable. Then populate the stringlist with the
  setStringList() and manage checked items with property checkedStringList. The getCheckedStringList() and
  setItemsCheckable() only work if the model of the view is a tkStringListModel.
  \ingroup toolkit
  \ingroup widget_toolkit
*/

#include "stringlistview.h"
#include "stringlistmodel.h"

#include <QAction>
#include <QMenu>

using namespace Views;

StringListView::StringListView( QWidget * parent )
        : ListView(parent)
{
}

StringListView::~StringListView()
{
}

QVariant StringListView::getCheckedStringList() const
{
    Q_ASSERT_X( static_cast<StringListModel*>(this->model()), "StringListView::getCheckedStringList()",
                "This member can only be used if the model is a tkStringListModel.");
    StringListModel * m = static_cast<StringListModel*>(this->model());
    if (!m)
        return QVariant();
    return m->getCheckedItems();
}

void StringListView::setCheckedStringList( const QVariant & list )
{
    Q_ASSERT_X( static_cast<StringListModel*>(this->model()), "StringListView::setCheckedStringList()",
                "This member can only be used if the model is a tkStringListModel.");
    StringListModel * m = static_cast<StringListModel*>(this->model());
    if (!m)
        return ;
    m->setCheckedItems( list.toStringList() );
}

void StringListView::setItemsCheckable( bool state )
{
    StringListModel * m = static_cast<StringListModel*>(this->model());
    if (m)
        m->setCheckable(state);
}
