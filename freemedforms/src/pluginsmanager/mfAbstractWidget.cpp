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
#include "mfAbstractWidget.h"
#include <mfObject.h>

#include <QLocale>
#include <QEvent>

mfAbstractWidget::mfAbstractWidget( mfObject * mfo, mfAbstractWidget * parent )
          : QWidget( parent ), m_Parent ( parent ), m_Object( mfo ), m_Label( 0 )
{
     m_OldTrans = QLocale().name().left( 2 );
}

void mfAbstractWidget::createLabel( mfObject * mfo, Qt::Alignment horizAlign )
{
     m_Label = new QLabel( this );
     m_Label->setFrameStyle( ItemLabelFrame );
     m_Label->setText( mfo->param( mfObject::Param_Label ).toString() );
     m_Label->setAlignment( Qt::AlignTop | horizAlign );
     m_Label->setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Expanding );
     m_Label->setWordWrap( true );
}

QBoxLayout * mfAbstractWidget::getBoxLayout( mfObject * mfo, QWidget * parent )
{
     QBoxLayout * hb;
     if ( mfo->options() & mfObjectFundamental::LabelOnTop )
     {
          hb = new QBoxLayout( QBoxLayout::TopToBottom, parent );
          createLabel( mfo, Qt::AlignTop | Qt::AlignLeft );
          hb->setSpacing( 0 );
     }
     else
     {
          hb = new QBoxLayout( QBoxLayout::LeftToRight, parent );
          createLabel( mfo, Qt::AlignTop | Qt::AlignRight );
     }

     if ( mfo->options() & mfObjectFundamental::BoldLabel )
     {
          QFont font = m_Label->font();
          font.setBold( true );
          m_Label->setFont( font );
     }

     return hb;
}


void mfAbstractWidget::changeEvent( QEvent * event )
{
     if ( ( event->type() == QEvent::LanguageChange ) &&
               ( m_OldTrans != QLocale().name().left( 2 ) ) )
     {
          m_OldTrans = QLocale().name().left( 2 );
          retranslateUi( QLocale().name().left( 2 ) );
          event->accept();
     }
     else
          QWidget::changeEvent( event );
}


