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
#ifndef TKSTRINGLISTVIEW_H
#define TKSTRINGLISTVIEW_H

#include <listviewplugin/listview_exporter.h>
#include <listviewplugin/listview.h>

//#include <QListView>
#include <QEvent>
#include <QObject>
#include <QStringListModel>

/**
 * \file tkStringListView.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.3
 * \date 08 March 2009
*/

namespace Views {

class LISTVIEW_EXPORT StringListView : public ListView //QListView
{
    Q_OBJECT
    Q_PROPERTY( QVariant stringList         READ getStringList         WRITE setStringList        USER true )
    Q_PROPERTY( QVariant checkedStringList  READ getCheckedStringList  WRITE setCheckedStringList USER true )

public:
    StringListView( QWidget * parent = 0 );
    ~StringListView();

    QVariant getStringList() const              { return static_cast<QStringListModel*>(this->model())->stringList(); }
    void setStringList( const QVariant & list ) { static_cast<QStringListModel*>(this->model())->setStringList( list.toStringList()); }

    void setItemsCheckable( bool state );
    QVariant getCheckedStringList() const;
    void setCheckedStringList( const QVariant & list );

};

}  // End Views
#endif // TKSTRINGLISTVIEW_H
