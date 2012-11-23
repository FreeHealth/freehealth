/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
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
 *   This code is largely inspired of the Qt example : Text Edit           *
 *   Adaptations to FreeMedForms by : Eric Maeker, MD                      *
 *   eric.maeker@gmail.com                                                   *
 ***************************************************************************/

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MFRICHTEXTEDITORWIDGET_H
#define MFRICHTEXTEDITORWIDGET_H

#include <mfObject.h>
#include <mfFormWidgetInterface.h>
#include <mfBaseInterface.h>
#include <mfAbstractWidget.h>
#include <mfExporter.h>
#include <tkRichTextEditor.h>

#include <QObject>

/**
 * \file mfRichTextEditorWidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.2
 * \date 07 March 2009
*/

/**
  \brief Rich Text Editor Plugin for FreeMedForms.
*/
class mfRichTextEditorWidget : public mfFormWidgetInterface
{
     Q_OBJECT
     Q_INTERFACES( mfBaseInterface mfFormWidgetInterface );

public:
     mfRichTextEditorWidget();

     QStringList   widgets() const;
     bool          isContainer( const int idInStringList ) const;
     mfAbstractWidget * getWidget( mfObject * mfo, mfAbstractWidget * parent = 0 ) const;
};



//--------------------------------------------------------------------------------------------------------
//------------------------------------ mfRichTextEditor implementation -----------------------------------
//--------------------------------------------------------------------------------------------------------
class mfRichTextEditor : public mfAbstractWidget
{
     Q_OBJECT
public:
     mfRichTextEditor( mfObject * mfo , mfAbstractWidget *parent = 0 );
     ~mfRichTextEditor();

public Q_SLOTS:
     void retranslateUi( const QString & );

private Q_SLOTS:
     void updateObject();
     void updateWidget();

 private:
     tkRichTextEditor::Types getType( mfObject * mfo );

 private:
     tkRichTextEditor * d;
};

#endif  // MFRICHTEXTEDITORWIDGET_H
