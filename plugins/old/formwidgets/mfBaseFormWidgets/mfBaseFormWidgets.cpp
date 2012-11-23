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
#include "mfBaseFormWidgets.h"

#include <QStringList>
#include <QMessageBox>


mfBaseFormWidgets::mfBaseFormWidgets()
{
     // define plugins informations
     mPluginInfos.Caption = tr( "Form's Base Widgets" );
     mPluginInfos.Description = tr( "Plugin for form's widget : base widgets." );
     mPluginInfos.Author = "Eric Maeker,MD <eric.maeker@gmail.com>";
     mPluginInfos.Type = mfBaseInterface::iFormWidget;
     mPluginInfos.Name = PLUGIN_NAME;
     mPluginInfos.Version = "1.0.0";
     mPluginInfos.Enabled = false;
}


QStringList mfBaseFormWidgets::widgets() const
{
     return mfObject::getDefaultTypeNames();
}

bool mfBaseFormWidgets::isContainer( const int idInStringList ) const
{
     return ( idInStringList == mfObject::Type_Form ) ||
            ( idInStringList == mfObject::Type_Group );
}

mfAbstractWidget * mfBaseFormWidgets::getWidget( mfObject * mfo, mfAbstractWidget * parent ) const
{
     // Create asked form's widget
     int t = mfo->type();
     switch ( t )
     {
          case mfObject::Type_Form :
               return new mfBaseForm( mfo, parent );
               break;
          case mfObject::Type_Group :
               return new mfBaseGroup( mfo, parent );
               break;
          case mfObject::Type_Check :
               return new mfBaseCheck( mfo, parent );
               break;
          case mfObject::Type_Radio :
               return new mfBaseRadio( mfo, parent );
               break;
          case mfObject::Type_ShortText :
          case mfObject::Type_LongText :
               return new mfBaseSimpleText( mfo, parent );
               break;
          case mfObject::Type_HelpText :
               return new mfBaseHelpText( mfo, parent );
               break;
          case mfObject::Type_MultiList :
          case mfObject::Type_UniqueList :
               return new mfBaseList( mfo, parent );
               break;
          case mfObject::Type_Combo :
               return new mfBaseCombo( mfo, parent );
               break;
          case mfObject::Type_Date :
               return new mfBaseDate( mfo, parent );
               break;
          case mfObject::Type_Spin :
               return new mfBaseSpin( mfo, parent );
               break;
          case mfObject::Type_Button :
               return new mfBaseButton( mfo, parent );
               break;
          default:
          {
               return 0;
               break;
          }
     }
     return 0;
}



//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfBaseForm implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
mfBaseForm::mfBaseForm( mfObject * mfo, mfAbstractWidget * parent )
          : mfAbstractWidget( mfo, parent ), m_ContainerLayout( 0 )
{
     QVBoxLayout *mainLayout = new QVBoxLayout( this );
     QWidget *mainWidget = new QWidget;
     mainLayout->addWidget( mainWidget );
     mainLayout->addStretch();

     m_ContainerLayout = new QGridLayout( mainWidget );
     mfAbstractWidget::createLabel( mfo, Qt::AlignCenter );
     m_Label->setFrameStyle( FormLabelFrame );
     QFont font = m_Label->font();
     font.setBold( true );
     m_Label->setFont( font );
     QSizePolicy sizePolicy = m_Label->sizePolicy();
     sizePolicy.setVerticalPolicy( QSizePolicy::Fixed );
     m_Label->setSizePolicy( sizePolicy );

     // Retrieve the number of columns for the gridlayout (lays in "option")
     numberColumns = mfo->getNumberOfColumn( 1 );
     m_ContainerLayout->addWidget( m_Label, 0, 0,  1, numberColumns );
     i = numberColumns;
     row = 0;
     col = 0;
}

mfBaseForm::~mfBaseForm()
{
}

void mfBaseForm::addWidgetToContainer( mfAbstractWidget * widget )
{
     if ( !widget ) return;
     if ( !m_ContainerLayout ) return;
     if ( widget->getObject()->type() == mfObject::Type_Form ) return;
     col = ( i % numberColumns );
     row = ( i / numberColumns );
     m_ContainerLayout->addWidget( widget , row, col );
     i++;
}

void mfBaseForm::retranslateUi( const QString & )
{
     m_Label->setText( m_Object->label() );
}



//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfBaseGroup implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
mfBaseGroup::mfBaseGroup( mfObject * mfo, mfAbstractWidget * parent )
          : mfAbstractWidget( mfo, parent ), m_Group( 0 ), m_ContainerLayout( 0 )
{
     QVBoxLayout * vblayout = new QVBoxLayout( this );
     m_Group = new QGroupBox( this );
     m_Group->setTitle( mfo->label() );
     vblayout->addWidget( m_Group );
     this->setLayout( vblayout );

     // Retrieve the number of columns for the gridlayout (lays in "option")
     numberColumns = mfo->getNumberOfColumn( 2 );

     // Create the gridlayout with all the widgets
     m_ContainerLayout = new QGridLayout( m_Group );
     i = 0;
     row = 0;
     col = 0;

     if ( mfo->options() & mfObjectFundamental::Checkable )
     {
          m_Group->setCheckable( true );
          m_Group->setChecked( mfo->options() & mfObjectFundamental::Checked );
          connect( m_Group, SIGNAL( clicked( bool ) ),
                   this,    SLOT  ( updateObject( bool ) ) );
          connect( m_Object, SIGNAL( valueChanged() ),
                   this,     SLOT  ( updateWidget() ) );
     }
     m_Group->setLayout( m_ContainerLayout );
}

mfBaseGroup::~mfBaseGroup()
{
}

void mfBaseGroup::addWidgetToContainer( mfAbstractWidget * widget )
{
     if ( !widget ) return;
     if ( !m_ContainerLayout ) return;
     if ( widget->getObject()->type() == mfObject::Type_Form ) return;

     col = ( i % numberColumns );
     row = ( i / numberColumns );
     m_ContainerLayout->addWidget( widget , row, col );
     i++;
}


void mfBaseGroup::updateObject( bool state )
{
     m_Object->disconnect();
     m_Object->selectedValueChangedTo( state );
     connect( m_Object, SIGNAL( valueChanged() ),
              this,     SLOT  ( updateWidget() ) );
}

void mfBaseGroup::updateWidget()
{
     m_Group->disconnect();
     m_Group->setChecked( m_Object->isChecked() );
     connect( m_Group, SIGNAL( clicked( bool ) ),
              this,    SLOT  ( updateObject( bool ) ) );
}

void mfBaseGroup::retranslateUi( const QString & )
{
     m_Group->setTitle( m_Object->label() );
}


//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- mfBaseCheck ----------------------------------------------
//--------------------------------------------------------------------------------------------------------
mfBaseCheck::mfBaseCheck( mfObject * mfo, mfAbstractWidget * parent )
          : mfAbstractWidget( mfo, parent ), m_Check( 0 )
{
     QHBoxLayout * hb = new QHBoxLayout( this );
     // Add Buttons
     m_Check = new QCheckBox ( this );
     m_Check->setObjectName( "Checkbox_" + mfo->name() );
     m_Check->setText( mfo->label() );
     hb->addWidget( m_Check );
     // Connect cliked to object
     connect ( m_Check,  SIGNAL( clicked( bool ) ),
               this,     SLOT( updateObject( bool ) ) );
     connect ( m_Object, SIGNAL( valueChanged() ),
               this,     SLOT ( updateWidget() ) );
     // if selected data exists fill the widget with
     if ( mfo->value() != QVariant() )
          updateWidget();
}

mfBaseCheck::~mfBaseCheck()
{
}

void mfBaseCheck::updateObject( bool chked )
{
     m_Object->selectedValueChangedTo( chked );
}

void mfBaseCheck::updateWidget()
{
     m_Check->setChecked( m_Object->isChecked() );
}

void mfBaseCheck::retranslateUi( const QString & )
{
     m_Check->setText( m_Object->label() );
}


//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- mfBaseRadio ----------------------------------------------
//--------------------------------------------------------------------------------------------------------
mfBaseRadio::mfBaseRadio( mfObject * mfo, mfAbstractWidget * parent )
          : mfAbstractWidget( mfo, parent )
{
     // Prepare Widget Layout and label
     QBoxLayout * hb = getBoxLayout( mfo, this );

     // Add QLabel
     m_Label->setSizePolicy( QSizePolicy::Preferred , QSizePolicy::Preferred );
     hb->addWidget( m_Label );

     if ( !( mfo->options() & mfObjectFundamental::LabelOnTop ) )
     {
          Qt::Alignment alignment = m_Label->alignment();
          alignment &= ~( Qt::AlignVertical_Mask );
          alignment |= Qt::AlignVCenter;
          m_Label->setAlignment( alignment );
     }

     // Add Buttons
     QGroupBox * gb = new QGroupBox( this );
     gb->setFlat( true );
     QSizePolicy sizePolicy = gb->sizePolicy();
     sizePolicy.setHorizontalPolicy( QSizePolicy::Fixed );
     gb->setSizePolicy( sizePolicy );
     QBoxLayout * radioLayout = 0;
     if ( mfo->options() & mfObjectFundamental::Horizontal )
          radioLayout = new QBoxLayout( QBoxLayout::LeftToRight, gb );
     else
          radioLayout = new QBoxLayout( QBoxLayout::TopToBottom, gb );
     radioLayout->setContentsMargins( 1, 0, 1, 0 );
     QRadioButton * rb = 0;
     int i = 0;
     foreach ( QString v, mfo->possibleValues() )
     {
          rb = new QRadioButton( this );
          rb->setObjectName( "Radio" );
          rb->setText( v );
          rb->setProperty ( "id" , i );
          i++;
          radioLayout->addWidget( rb );
          m_RadioList.append( rb );
          // Connect radio button enabled/disabled with mfObject
          connect( rb ,    SIGNAL( clicked( bool ) ),
                   this ,  SLOT  ( updateObject( bool ) ) );
          // Connect object value changed signal with radiobutton
          connect ( mfo,   SIGNAL( valueChanged() ),
                    this,  SLOT  ( updateWidget() ) );
     }
     hb->addWidget( gb );

     // if selected data exists fill the widget with
     if ( mfo->value() != QVariant() )
          updateWidget();

}

mfBaseRadio::~mfBaseRadio()
{ }

void mfBaseRadio::updateObject( bool )
{
     QRadioButton * but = qobject_cast<QRadioButton *>( sender() );
     m_Object->selectedValueChangedTo( but->property( "id" ).toInt() );
}

void mfBaseRadio::updateWidget()
{
     QList<QRadioButton*> finded = this->findChildren<QRadioButton *>( "Radio" );
     if ( !finded.count() ) return;

     foreach( QRadioButton* but , finded )
     {
          if ( but->property( "id" ).toInt() == *m_Object->selectedValuesIds().constBegin() )
               but->setChecked( true );
          else
               but->setChecked( false );
     }
}

void mfBaseRadio::retranslateUi( const QString & lang )
{
     m_Label->setText( m_Object->label() );

     if ( m_RadioList.size() )
     {
          QStringList list = m_Object->possibleValues();
          if ( list.count() != m_RadioList.count() )
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
          foreach ( QRadioButton * button, m_RadioList )
          {
               button->setText( list.at( i ) );
               i++;
          }
     }
}


//--------------------------------------------------------------------------------------------------------
//------------------------------------------- mfBaseSimpleText -------------------------------------------
//--------------------------------------------------------------------------------------------------------
mfBaseSimpleText::mfBaseSimpleText( mfObject * mfo, mfAbstractWidget * parent )
          : mfAbstractWidget( mfo, parent ), m_Line( 0 ), m_Text( 0 )
{
     // Prepare Widget Layout and label
     QBoxLayout * hb = getBoxLayout( mfo, this );

     // Add QLabel
     hb->addWidget( m_Label );

     bool shortText = ( mfo->type() == mfObject::Type_ShortText );

     // Add List and manage size
     if ( shortText )
     {
          if ( !( mfo->options() & mfObjectFundamental::LabelOnTop ) )
          {
               Qt::Alignment alignment = m_Label->alignment();
               alignment &= ~( Qt::AlignVertical_Mask );
               alignment |= Qt::AlignVCenter;
               m_Label->setAlignment( alignment );
          }

          m_Line = new QLineEdit( this );
          m_Line->setObjectName( "Line_" + mfo->name() );
          m_Line->setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Fixed );
          m_Line->setInputMask( m_Object->mask() );
          m_Line->setCursorPosition( 0 );
          hb->addWidget( m_Line );
          // Connect list selection changed with mfObject value changed
          connect( m_Line,       SIGNAL( textChanged( const QString & ) ),
                   this ,      SLOT  ( updateObject( const QString & ) ) );
          connect( mfo,        SIGNAL( valueChanged() ),
                   this ,      SLOT  ( updateWidget() ) );

     }
     else
     {
          m_Text = new QTextEdit( this );
          m_Text->setObjectName( "Text_" + mfo->name() );
          m_Text->setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Expanding );
          hb->addWidget( m_Text );
          // Connect list selection changed with mfObject value changed
          connect( m_Text,       SIGNAL( textChanged () ),
                   this ,      SLOT  ( updateObject() ) );
          connect( mfo,        SIGNAL( valueChanged() ),
                   this ,      SLOT  ( updateWidget() ) );

     }

     // if selected data exists fill the widget with
     if ( mfo->value() != QVariant() )
          updateWidget();

}

mfBaseSimpleText::~mfBaseSimpleText()
{
}

void mfBaseSimpleText::updateObject( const QString & value )
{
     m_Object->disconnect();
     m_Object->selectedValueChangedTo( value );
     connect( m_Object,   SIGNAL( valueChanged() ),
              this ,      SLOT  ( updateWidget() ) );
}

void mfBaseSimpleText::updateObject()
{
     if ( !m_Text ) return;
     m_Object->disconnect();
     m_Object->selectedValueChangedTo( m_Text->toPlainText() );
     connect( m_Object,   SIGNAL( valueChanged() ),
              this ,      SLOT  ( updateWidget() ) );

}

void mfBaseSimpleText::updateWidget()
{
     // Get text object
     if ( m_Line )
     {
          m_Line->disconnect();
          m_Line->setText( m_Object->value().toString() );
          m_Line->repaint();
          connect( m_Line,  SIGNAL( textChanged( const QString & ) ),
                   this ,   SLOT  ( updateObject( const QString & ) ) );
     }
     else if ( m_Text )
     {
          m_Text->disconnect();
          m_Text->setPlainText( m_Object->value().toString() );
          connect( m_Text,  SIGNAL( textChanged () ),
                   this ,   SLOT  ( updateObject() ) );
     }
}

void mfBaseSimpleText::retranslateUi( const QString & )
{
     m_Label->setText( m_Object->label() );
}


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- mfBaseHelpText -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
mfBaseHelpText::mfBaseHelpText( mfObject * mfo, mfAbstractWidget * parent )
          : mfAbstractWidget( mfo, parent )
{
     QHBoxLayout * hb = new QHBoxLayout( this );
     // Add QLabel
     createLabel( mfo, Qt::AlignJustify );
     // Setting objectName to hide/show via a simple option button
     m_Label->setObjectName( "Help" );
     hb->addWidget( m_Label );
}

mfBaseHelpText::~mfBaseHelpText()
{
}

void mfBaseHelpText::retranslateUi( const QString & )
{
     m_Label->setText( m_Object->label() );
}


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- mfBaseLists --------------------------------------------------
//--------------------------------------------------------------------------------------------------------
mfBaseList::mfBaseList( mfObject * mfo, mfAbstractWidget * parent )
          : mfAbstractWidget( mfo, parent ), m_List( 0 )
{
     // Prepare Widget Layout and label
     QBoxLayout * hb = getBoxLayout( mfo, this );

     // Add QLabel
     hb->addWidget( m_Label );

     // Add List and manage size
     m_List = new QListWidget( this );
     m_List->setObjectName( "List_" + mfo->name() );
     m_List->setUniformItemSizes( true );
     m_List->addItems( mfo->possibleValues() );
     m_List->setSortingEnabled( false );
     m_List->setAlternatingRowColors( true );
     m_List->setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Expanding );

     bool multi = ( m_Object->type() == mfObject::Type_MultiList );

     if ( multi )
          m_List->setSelectionMode( QAbstractItemView::MultiSelection );
     else
          m_List->setSelectionMode( QAbstractItemView::SingleSelection );

     hb->addWidget( m_List );

     // Connect list selection changed with mfObject
     connect( m_List, SIGNAL( itemSelectionChanged() ),
              this ,      SLOT  ( updateObject() ) );
     connect( mfo,        SIGNAL( valueChanged() ),
              this,       SLOT  ( updateWidget() ) );

     // if selected data exists fill the widget with
     if ( mfo->value() != QVariant() )
          updateWidget();
}

mfBaseList::~mfBaseList()
{
}

void mfBaseList::updateObject()
{
     if ( !m_List ) return;

     QSet<int> values;
     foreach( QListWidgetItem* item, m_List->selectedItems() )
     {
          values << m_List->row( item );
     }
     m_Object->selectedValueChangedTo( values );
}

void mfBaseList::updateWidget()
{
     if ( !m_List ) return;

     m_List->disconnect();

     m_List->clearSelection();
     foreach( int idx, m_Object->selectedValuesIds() )
     m_List->item( idx )->setSelected( true );

     connect( m_List, SIGNAL( itemSelectionChanged() ),
              this ,      SLOT  ( updateObject() ) );

}

void mfBaseList::retranslateUi( const QString & lang )
{
     m_Label->setText( m_Object->label() );

     if ( m_List )
     {
          QStringList list = m_Object->possibleValues();
          if ( list.count() != m_List->count() )
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
          for ( i = 0; i < m_List->count(); i++ )
               m_List->item( i )->setText( list[i] );
     }
}

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- mfBaseCombo --------------------------------------------------
//--------------------------------------------------------------------------------------------------------
mfBaseCombo::mfBaseCombo( mfObject * mfo, mfAbstractWidget * parent )
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
     m_Combo->addItems( mfo->possibleValues() );
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
}

mfBaseCombo::~mfBaseCombo()
{
}

void mfBaseCombo::updateObject( int id )
{
     m_Object->selectedValueChangedTo( id );
}

void mfBaseCombo::updateWidget()
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

void mfBaseCombo::retranslateUi( const QString & lang )
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
          // refresh combo items
          m_Combo->clear();
          m_Combo->addItems( m_Object->possibleValues() );
          // refresh selected value
          updateWidget();
     }

}

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- mfBaseDate ---------------------------------------------------
//--------------------------------------------------------------------------------------------------------
mfBaseDate::mfBaseDate( mfObject * mfo, mfAbstractWidget * parent )
          : mfAbstractWidget( mfo, parent ), m_Date( 0 )
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

     // Add Date selector and manage date format
     m_Date = new QDateTimeEdit( this );
     m_Date->setObjectName( "Date_" + m_Object->name() );
     m_Date->setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Fixed );
     m_Date->setDisplayFormat( mfo->dateFormat() );
     m_Date->setCalendarPopup( true );
     m_Date->setDateTime( mfo->dateTime() );
     hb->addWidget( m_Date );

     // Initialize mfo and dateedit with mfo options
     const QStringList & options = mfo->param( mfObject::Param_Options ).toStringList();
     if ( options.contains( "now" ) )
          m_Date->setDateTime( QDateTime::currentDateTime() );

     // Connect list selection changed with mfObject value changed
     connect( m_Date,   SIGNAL( dateTimeChanged ( const QDateTime & ) ),
              this ,  SLOT  ( updateObject( const QDateTime & ) ) );
     connect( m_Date,   SIGNAL( dateChanged ( const QDate & ) ),
              this ,  SLOT  ( updateObject( const QDate & ) ) );
     connect( mfo,    SIGNAL( valueChanged() ),
              this,   SLOT  ( updateWidget() ) );

     // if selected data exists fill the widget with
     if ( mfo->value() != QVariant() )
          updateWidget();

}

mfBaseDate::~mfBaseDate()
{
}

void mfBaseDate::updateObject( const QDateTime & datetime )
{
     m_Object->selectedValueChangedTo( datetime );
}

void mfBaseDate::updateObject( const QDate & date )
{
     m_Object->selectedValueChangedTo( QDateTime( date ) );
}

void mfBaseDate::updateWidget()
{
     if ( !m_Date ) return;

     m_Date->disconnect();

     m_Date->setDateTime( m_Object->dateTime() );

     connect( m_Date, SIGNAL( dateTimeChanged ( const QDateTime & ) ),
              this ,  SLOT  ( updateObject( const QDateTime & ) ) );
     connect( m_Date, SIGNAL( dateChanged ( const QDate & ) ),
              this ,  SLOT  ( updateObject( const QDate & ) ) );
}

void mfBaseDate::retranslateUi( const QString & )
{
     m_Label->setText( m_Object->label() );

}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------ mfBaseSpin --------------------------------------------------
//--------------------------------------------------------------------------------------------------------
mfBaseSpin::mfBaseSpin( mfObject * mfo, mfAbstractWidget * parent )
          : mfAbstractWidget( mfo, parent ), m_Spin( 0 )
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

     // Add Date selector and manage date format
     m_Spin = new QSpinBox( this );
     m_Spin->setObjectName( "Spin_" + m_Object->name() );
     m_Spin->setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Fixed );
     m_Spin->setValue( mfo->value().toInt() );
     hb->addWidget( m_Spin );

     // Connect list selection changed with mfObject value changed
     connect( m_Spin,   SIGNAL( valueChanged ( int ) ),
              this ,  SLOT  ( updateObject( int ) ) );
     connect( mfo,    SIGNAL( valueChanged() ),
              this,   SLOT  ( updateWidget() ) );

     // if selected data exists fill the widget with
     if ( mfo->value() != QVariant() )
          updateWidget();
}

mfBaseSpin::~mfBaseSpin()
{}

void mfBaseSpin::updateObject( int val )
{
     m_Object->disconnect();
     m_Object->selectedValueChangedTo( val );
     connect( m_Object,    SIGNAL( valueChanged() ),
              this,   SLOT  ( updateWidget() ) );

}

void mfBaseSpin::updateWidget()
{
     if ( !m_Spin ) return;

     m_Spin->disconnect();

     m_Spin->setValue( m_Object->value().toInt() );

     connect( m_Spin,   SIGNAL( valueChanged ( int ) ),
              this ,  SLOT  ( updateObject( int ) ) );
}

void mfBaseSpin::retranslateUi( const QString & )
{
     m_Label->setText( m_Object->label() );
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------ mfBaseButton ------------------------------------------------
//--------------------------------------------------------------------------------------------------------
mfBaseButton::mfBaseButton( mfObject * mfo, mfAbstractWidget * parent )
          : mfAbstractWidget( mfo, parent ), m_Button( 0 )
{
     QHBoxLayout * hb = new QHBoxLayout( this );
     // Add QLabel
     hb->addStretch();

     m_Button = new QPushButton( this );
     m_Button->setObjectName( mfo->name() );
     m_Button->setText( mfo->label() );
     m_Button->setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Fixed );
     hb->addWidget( m_Button );

     connect( m_Button, SIGNAL( clicked() ) , this , SLOT( buttonClicked() ) );
}

mfBaseButton::~mfBaseButton()
{}

void mfBaseButton::buttonClicked()
{
     m_Object->runScript();
}

void mfBaseButton::retranslateUi( const QString & )
{
     m_Button->setText( m_Object->label() );
}

//--------------------------------------------------------------------------------------------------------
//---------------------------------------- Export class as plugin ----------------------------------------
//--------------------------------------------------------------------------------------------------------
Q_EXPORT_PLUGIN2( mf_baseformwidgets, mfBaseFormWidgets )
