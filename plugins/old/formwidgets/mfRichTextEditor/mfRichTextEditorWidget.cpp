/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "mfRichTextEditorWidget.h"

// include freemedforms headers
#include <mfCore.h>

#include <tkRichTextEditor.h>

#include <QGridLayout>
#include <QTextEdit>


namespace mfRichTextEditorWidgetPrivateConstants {
    const char* const OPTION_WITHLOAD    = "withload";
    const char* const OPTION_WITHSAVE    = "withsave";
    const char* const OPTION_WITHTABLES  = "withtables";
    const char* const OPTION_ALLOPTIONS  = "alloptions";
}

using namespace mfRichTextEditorWidgetPrivateConstants;


//--------------------------------------------------------------------------------------------------------
//----------------------------------- Plugin interface for FreeMedForms ----------------------------------
//--------------------------------------------------------------------------------------------------------
mfRichTextEditorWidget::mfRichTextEditorWidget()
{
     // define plugins informations
     mPluginInfos.Caption = tr( "Form's Rich Text Editor" );
     mPluginInfos.Description = tr( "Plugin for form's widget : rich text editor" );
     mPluginInfos.Author = "Eric Maeker,MD <eric.maeker@gmail.com>";
     mPluginInfos.Type = mfBaseInterface::iFormWidget;
     mPluginInfos.Name = PLUGIN_NAME;
     mPluginInfos.Version = "1.0.0";
     mPluginInfos.Enabled = false;
}

QStringList mfRichTextEditorWidget::widgets() const
{
     return QStringList() << "simplerichtext" << "richtext" << "fullrichtext";
}

bool mfRichTextEditorWidget::isContainer( const int ) const
{
     return false;
}

mfAbstractWidget * mfRichTextEditorWidget::getWidget( mfObject * mfo, mfAbstractWidget * parent ) const
{
     return new mfRichTextEditor( mfo, parent );
}


//--------------------------------------------------------------------------------------------------------
//------------------------------------ mfRichTextEditor implementation -----------------------------------
//--------------------------------------------------------------------------------------------------------
mfRichTextEditor::mfRichTextEditor( mfObject *mfo , mfAbstractWidget *parent )
          : mfAbstractWidget( mfo , parent )
{
     // Connect Widget to object and vice versa
    QGridLayout *l = new QGridLayout( this );
    d = new tkRichTextEditor( this, getType( mfo ) ) ;
//    setFocusProxy(d->textEdit());

    l->addWidget( d );
    l->setMargin( 0 );
    l->setSpacing( 0 );
    connect( d->textEdit(), SIGNAL( textChanged() ),
             this, SLOT( updateObject() ) );
    connect( m_Object, SIGNAL( valueChanged() ),
             this, SLOT( updateWidget() ) );

    // Don't forget to tell mfObject the type of selected value that is managed by this plugin
     m_Object->setPluginSelectedValueType( mfObject::PlugType_String );

     // if selected data exists fill the widget with
     if ( mfo->value() != QVariant() )
          updateObject();
}

tkRichTextEditor::Types mfRichTextEditor::getType( mfObject * mfo )
{
    tkRichTextEditor::Types t;
    // Determine type
    if ( mfo->param( mfObject::Param_Type ).toString()
        .compare( "fullrichtext", Qt::CaseInsensitive ) == 0 )
        t |= tkRichTextEditor::Full;

    // And options
    const QStringList &options = mfo->param( mfObject::Param_Options ).toStringList();
    if ( options.contains( OPTION_WITHLOAD, Qt::CaseInsensitive ) )
        t |= tkRichTextEditor::WithIO;
    if ( options.contains( OPTION_WITHSAVE, Qt::CaseInsensitive ) )
        t |= tkRichTextEditor::WithIO;
    if ( options.contains( OPTION_WITHTABLES, Qt::CaseInsensitive ) )
        t |= tkRichTextEditor::WithTables;
    if ( options.contains( OPTION_ALLOPTIONS, Qt::CaseInsensitive ) )
        t = tkRichTextEditor::Full;
    return t;
}

mfRichTextEditor::~mfRichTextEditor()
{
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------- File Open / Save -------------------------------------
//--------------------------------------------------------------------------------------------------------
void mfRichTextEditor::updateObject()
{
     m_Object->disconnect();
     m_Object->selectedValueChangedTo( d->textEdit()->toHtml() );
     connect( m_Object, SIGNAL( valueChanged() ),
              this, SLOT( updateWidget() ) );
}

void mfRichTextEditor::updateWidget()
{
     d->textEdit()->disconnect( SIGNAL( textChanged() ) );
     d->textEdit()->setHtml( m_Object->value().toString() );
     connect( d->textEdit(), SIGNAL( textChanged() ),
              this, SLOT( updateObject() ) );
}

//--------------------------------------------------------------------------------------------------------
//---------------------------------------------- Retranslate UI ------------------------------------------
//--------------------------------------------------------------------------------------------------------
void mfRichTextEditor::retranslateUi( const QString & )
{
}

//--------------------------------------------------------------------------------------------------------
//---------------------------------------- Export class as plugin ----------------------------------------
//--------------------------------------------------------------------------------------------------------
Q_EXPORT_PLUGIN2( mf_richtextwidgets, mfRichTextEditorWidget )
