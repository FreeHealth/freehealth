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
 *   This code is inspired of the Qt example : Text Edit                   *
 *   Adaptations to FreeMedForms and improvments by : Eric Maeker, MD      *
 *   eric.maeker@free.fr                                                   *
 ***************************************************************************/

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TKRICHTEXTEDITOR_H
#define TKRICHTEXTEDITOR_H

#include <tkExporter.h>
class tkRichTextEditorPrivate;

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QFocusEvent>
class QMenu;

/**
 * \file tkRichTextEditorWidget.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.4
 * \date 12 June 2009
*/

//--------------------------------------------------------------------------------------------------------
//------------------------------------ mfRichTextEditor implementation -----------------------------------
//--------------------------------------------------------------------------------------------------------
/**
  \class tkRichTextEditor
  \todo Document code.
  \ingroup toolkit
  \ingroup widget_toolkit
*/
class Q_TK_EXPORT tkRichTextEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( QString html READ getHtml WRITE setHtml USER true)

public:
    /** */
    enum Type
    {
        VerySimple = 0x00000000,
        Simple     = 0x00000001,
        Full       = 0x00000010,
        WithTables = 0x00000100,
        WithLoad   = 0x00001000,
        WithSave   = 0x00010000
                 };
    Q_DECLARE_FLAGS( Types, Type );


    /** */
    tkRichTextEditor( QWidget *parent = 0, Types type = Full);
    ~tkRichTextEditor();

    /** */
    virtual QTextEdit *textEdit(); // this pointer is provided for slot/signals connections

    /** */
    QString getHtml()                        { return textEdit()->toHtml(); }
    /** */
    void    setHtml( const QString & html)   { textEdit()->setHtml( html ); }
    /** */
    void setTypes( Types types );

protected:
    /** */
    virtual void changeEvent(QEvent *e);
    /** */
    virtual QMenu* getContextMenu();

private:
    tkRichTextEditorPrivate *d;
    friend class tkRichTextEditorPrivate;

};
Q_DECLARE_OPERATORS_FOR_FLAGS( tkRichTextEditor::Types )

#endif // TKRICHTEXTEDITOR_H
