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
#include "tkUserRightsWidget.h"

#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QEvent>

tkUserRightsWidget::tkUserRightsWidget( QWidget * parent )
    : QListWidget( parent )
{
    connect( this, SIGNAL(itemActivated(QListWidgetItem*)),
             this, SLOT( on_m_RightsListWidget_itemActivated(QListWidgetItem*)) );
    connect( this, SIGNAL(itemClicked(QListWidgetItem*)),
             this, SLOT( on_m_RightsListWidget_itemActivated(QListWidgetItem*)) );
    retranslate();
}

void tkUserRightsWidget::on_m_RightsListWidget_itemActivated( QListWidgetItem *item )
{
    if ( ! item )
        return;

    // if no rights is checked ans user clicked another rights --> pass
    if ( ( this->item(0)->checkState() == Qt::Checked ) &&
         (item != this->item(0) ) )
        return;

    bool resetRights = false;

    // change the checkstate of the clicked item
    if ( item->checkState() == Qt::Checked ) {
        item->setCheckState( Qt::Unchecked );
    } else {
        item->setCheckState( Qt::Checked );
    }
    Qt::CheckState s = item->checkState();

    // specific items (No rights, ReadAll, WriteAll)
    if ( ( item->data( Qt::UserRole ).toInt() == User::NoRights ) &&
         ( s == Qt::Checked ) ) {
        int i;
        for( i=0; i < count(); ++i)
            this->item( i )->setCheckState( Qt::Unchecked );
        item->setCheckState( s );
    } else if ( ( item->data( Qt::UserRole ).toInt() == User::ReadAll ) &&
                ( s == Qt::Checked ) ) {
        resetRights = true;
    } else if ( ( item->data( Qt::UserRole ).toInt() == User::WriteAll ) &&
                ( s == Qt::Checked ) ) {
        resetRights = true;
    }

    // store result to the m_Rights
    m_Rights = User::NoRights;
    int i;
    for( i = 0; i < count(); ++i) {
        if ( this->item( i )->checkState() == Qt::Checked )
            m_Rights |= User::UserRights( this->item( i )->data( Qt::UserRole ).toInt() );
    }

    // if needed reset rights to the listwidget (ReadAll, WriteAll need that)
    if ( resetRights )
        setCurrentRightsToWidget();
}

void tkUserRightsWidget::retranslate()
{
    clear();
    addItems( QStringList()
              << tr( "No rigths" )
              << tr( "Can read own datas" )
              << tr( "Can read delegates datas" )
              << tr( "Can read all datas" )
              << tr( "Can write own datas" )
              << tr( "Can write delegates datas" )
              << tr( "Can write all datas" )
              << tr( "Can print" )
              << tr( "Can create new datas" )
              << tr( "Can delete datas" ) );

    QList<int> rightsFlags;
    rightsFlags
            << User::NoRights << User::ReadOwn << User::ReadDelegates << User::ReadAll
            << User::WriteOwn << User::WriteDelegates << User::WriteAll
            << User::Print << User::Create << User::Delete;
    int i;
    for( i = 0; i < count(); ++i) {
        item( i )->setFlags( Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
        item( i )->setData( Qt::UserRole, rightsFlags.at( i ) );
    }

    setCurrentRightsToWidget();
}

void tkUserRightsWidget::setCurrentRightsToWidget()
{
    int i;
    if ( m_Rights == User::NoRights ) {
        // User don't have any rights
        for( i = 0; i < count(); ++i)
            this->item( i )->setCheckState( Qt::Unchecked );
        this->item(0)->setCheckState( Qt::Checked );
    } else {
        // User have some rights
        for( i = 0; i < count(); ++i) {
            if ( m_Rights & User::UserRights( this->item( i )->data( Qt::UserRole ).toInt() ) )
                this->item( i )->setCheckState( Qt::Checked );
            else
                this->item( i )->setCheckState( Qt::Unchecked );
        }
    }
}

void tkUserRightsWidget::changeEvent(QEvent *e)
{
    if ( (e->type() == QEvent::LanguageChange) )
        retranslate();
}
