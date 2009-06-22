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
#ifndef MFABSTRACTWIDGET_H
#define MFABSTRACTWIDGET_H

#include "mfExporter.h"
#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QBoxLayout>
class mfObject;

/**
 * \file mfAbstractWidget.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.6
 * \date 12 novembre 2008
*/


class Q_FMF_EXPORT mfAbstractWidget : public QWidget
{
     Q_OBJECT
public:

     enum QFrameStyleForLabels { FormLabelFrame = QFrame::Panel | QFrame::Sunken,
                                 ItemLabelFrame = QFrame::NoFrame,
                                 HelpTextFrame =  QFrame::Panel | QFrame::Sunken
                               };

     mfAbstractWidget( mfObject * mfo, mfAbstractWidget * parent = 0 );
     virtual ~mfAbstractWidget() {}

     virtual void addWidgetToContainer( mfAbstractWidget * ) {}

     virtual void createLabel( mfObject * mfo, Qt::Alignment horizAlign = Qt::AlignRight );
     virtual QBoxLayout * getBoxLayout( mfObject * mfo, QWidget * parent );

     virtual void setParent( mfAbstractWidget * parent ) { m_Parent = parent; }
     virtual mfAbstractWidget * mfParent() const { return m_Parent; }

     virtual void setObject( mfObject * mfo ) { m_Object = mfo; }
     virtual mfObject * getObject() const { return m_Object; }

     virtual void changeEvent( QEvent * event ); // Used for dynamic translation

public Q_SLOTS:
     virtual void retranslateUi( const QString & ) = 0;

public:
     mfAbstractWidget * m_Parent;
     mfObject * m_Object;
     QLabel * m_Label;

private:
     QString m_OldTrans;
};

#endif
