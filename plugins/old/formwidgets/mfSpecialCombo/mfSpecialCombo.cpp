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
#include "mfSpecialCombo.h"

#include <QStringList>
#include <QFile>
#include <QDir>
#include <QMessageBox>


mfSpecialComboPlugin::mfSpecialComboPlugin()
{
     // define plugins informations
     mPluginInfos.Caption = tr( "Form's Special Combo Widgets" );
     mPluginInfos.Description = tr( "Plugin for form's widget : "
                                    "special combo can be populated with a file or a database." );
     mPluginInfos.Author = "Eric Maeker,MD <eric.maeker@gmail.com>";
     mPluginInfos.Type = mfBaseInterface::iFormWidget;
     mPluginInfos.Name = PLUGIN_NAME;
     mPluginInfos.Version = "0.0.1";
     mPluginInfos.Enabled = false;
}


QStringList mfSpecialComboPlugin::widgets() const
{
     return QStringList() << "specialcombo";
}

bool mfSpecialComboPlugin::isContainer( const int ) const
{
     return false;
}

mfAbstractWidget * mfSpecialComboPlugin::getWidget( mfObject * mfo, mfAbstractWidget * parent ) const
{
     return new mfSpecialComboWidget( mfo, parent );
}



//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfBaseForm implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
mfSpecialComboWidget::mfSpecialComboWidget( mfObject * mfo, mfAbstractWidget * parent )
          : mfAbstractWidget( mfo, parent ), m_Combo( 0 )
{
     // Prepare Widget Layout and label
     QBoxLayout * hb = getBoxLayout( mfo, this );

     // Add QLabel
     hb->addWidget( m_Label );
     if ( !( mfo->options() & mfObjectFundamental::LabelOnTop ) )
     {
          Qt::Alignment alignment = m_Label->alignment();
          alignment &= ~( Qt::AlignVertical_Mask );
          alignment |= Qt::AlignVCenter;
          m_Label->setAlignment( alignment );
     }

     // Add List and manage size
     m_Combo = new QComboBox( this );
     m_Combo->setObjectName( "Combo_" + m_Object->name() );
     populateCombo();
     hb->addWidget( m_Combo );
     if ( mfo->options() & mfObjectFundamental::SizePreferred )
          m_Combo->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
     else
          m_Combo->setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Fixed );

     // Connect list selection changed with mfObject value changed
     connect( m_Combo, SIGNAL( activated ( int ) ),
              this ,   SLOT  ( updateObject( int ) ) );
     connect( mfo,     SIGNAL( valueChanged() ),
              this,    SLOT( updateWidget() ) );

     // if selected data exists fill the widget with
     if ( mfo->value() != QVariant() )
          updateWidget();

     // Don't forget to tell mfObject the type of selected value that is managed by this plugin
     m_Object->setPluginSelectedValueType( mfObject::PlugType_Ids );

}

mfSpecialComboWidget::~mfSpecialComboWidget()
{
}



void mfSpecialComboWidget::populateCombo()
{
     // TODO : manage file change with QFileSystemWatcher for a dynamic refreshing of the combo items (should be as an option)
     QStringList options = m_Object->param( mfObject::Param_Options ).toStringList();

     QRegExp rx( "file=*" );
     rx.setPatternSyntax( QRegExp::Wildcard );
     rx.setCaseSensitivity( Qt::CaseInsensitive );

     // manage only file (for know)
     if ( !options.filter( rx ).count() ) return;

     // retreive absolute file name
     QString file = options.filter( rx ).at( 0 );
     file = file.mid( file.indexOf( "file=" ) + 5 );
     QDir rootDir = QFileInfo( m_Object->absoluteFileName() ).dir();
     if ( QFileInfo( file ).isRelative() )
          file = rootDir.filePath( file );

     // open file and populate combo
     QFile f( file );
     if ( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
          return;
     QString in = f.readAll();
     in.replace( "\r" , "" );
     if ( QFileInfo( file ).suffix().compare( "txt", Qt::CaseInsensitive ) == 0 )
     {
          QStringList lines = in.split( "\n" );
          QString lang = QLocale().name().left( 2 );

          foreach( QString s, lines )
          {
               QStringList list = s.split( "|" );
               if ( list.count() < 3 ) continue;
               m_Object->setParam( mfObject::Param_PossibleValues, list[0], lang, mfObject::AppendParam );
               m_Object->setParam( mfObject::Param_InterpretValues, list[1], lang, mfObject::AppendParam );
               m_Object->setParam( mfObject::Param_NumericalValues, list[2], lang, mfObject::AppendParam );
          }
          m_Combo->addItems( m_Object->possibleValues() );
     }

}



void mfSpecialComboWidget::updateWidget()
{
     if ( !m_Combo ) return;
     m_Combo->disconnect();

     if ( m_Object->selectedValuesIds().count() )
          m_Combo->setCurrentIndex( *m_Object->selectedValuesIds().constBegin() );
     else
          m_Combo->setCurrentIndex( 0 );

     connect( m_Combo,  SIGNAL( activated ( int ) ),
              this ,    SLOT  ( updateObject( int ) ) );
}



void mfSpecialComboWidget::updateObject( int id )
{
     m_Object->selectedValueChangedTo( id );
}



void mfSpecialComboWidget::retranslateUi( const QString & lang )
{
     m_Label->setText( m_Object->label() );
     if ( m_Combo )
     {
          QStringList list = m_Object->possibleValues();
          if ( list.count() != m_Combo->count() )
          {
               QMessageBox::warning( this, qApp->applicationName(),
                                     tr( "You asked to change the language of the form to %1.\n"
                                         "But this an error while reading translation of %2.\n"
                                         "Number of items of the translation (%3) are wrong." )
                                     .arg( lang, m_Object->name() ).arg( list.count() ),
                                     QMessageBox::Ok , QMessageBox::Ok );
               return;
          }
          int i = 0;
          for ( i = 0; i < m_Combo->count(); i++ )
               m_Combo->setItemText( i, list[i] );
     }

}

//--------------------------------------------------------------------------------------------------------
//---------------------------------------- Export class as plugin ----------------------------------------
//--------------------------------------------------------------------------------------------------------
Q_EXPORT_PLUGIN2( mf_specialcombo, mfSpecialComboPlugin )
