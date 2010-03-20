/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "mfObject.h"

#include <QDebug>
#include <QtScript>

#include <mfCore.h>
#include <mfPluginsManager.h>
#include <mfScriptor.h>

//--------------------------------------------------------------------------------------------------------
//---------------------------------------- Creator and destructor ----------------------------------------
//--------------------------------------------------------------------------------------------------------
mfObject::mfObject( mfObject * parent )
          : mfObjectFundamental( parent ), m_IsCompleted ( false )
{
}

mfObject::~mfObject()
{
     // delete all childs
     qDeleteAll( mfChildren() );
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------- DEBUGGING FUNCTIONS ----------------------------------------
//--------------------------------------------------------------------------------------------------------
#ifndef QT_NO_DEBUG_OUTPUT
QDebug operator<<( QDebug dbg, const mfObject * c )
{
     dbg.nospace() << *c;
     return dbg.space();
}

QDebug operator<<( QDebug dbg, const mfObject & c )
{
     dbg.nospace() << c.toDebug();
     return dbg.space();
}
#endif

QString mfObject::toDebug() const
{
     QString ret;
     ret = "(name: " + name() + ",\n";
     ret += "  type: " + typeNames[type()] + ",\n";
     ret += "  value: \"" + value().toString() + "\", ";
     ret += "  id: " + QString::number( id() ) + ",\n";
     ret += "  selectedvalues: (" + selectedValues().join( ", " ) + "),\n";
     ret += "  textedit: \"" + m_TextEditorsContent + "\",\n";
     if ( isModified() )  ret += "  isModified: true, ";
     else ret += "  isModified: false, ";
     if ( interpreted() )  ret += "interpreted: true, ";
     else ret += "interpreted: false, ";
     if ( completed() )  ret += "completed: true, ";
     else ret += "completed: false, ";
     ret += "\n  absoluteFileName:" + absoluteFileName() + ")";

     return ret;
}

//--------------------------------------------------------------------------------------------------------
//-------------------------------------------- OnLoadScripting -------------------------------------------
//--------------------------------------------------------------------------------------------------------
void mfObject::runOnLoadScript( mfObject * mfo )
{
     mfObject * object = mfo;
     if ( !object ) object = this;
     mfScriptor * scriptor = mfCore::scriptor();

     if ( !param( mfObject::Param_OnLoadScript ).isNull() )
     {
          scriptor->setObject( object );
          scriptor->run( Param_OnLoadScript );
     }
     foreach( mfObject * chd, object->mfChildren() )
     runOnLoadScript( chd );
}

void mfObject::runScript()
{
     // This function is used by button widget
     mfScriptor * scriptor = mfCore::scriptor();

     if ( !param( mfObject::Param_Script ).isNull() )
     {
          scriptor->setObject( this );
          scriptor->run( Param_Script );
     }
}


void mfObject::setModified( const QVariant & state )
{
     m_IsModified = state.toBool();

     if (m_IsModified)
     {
          if ( mfParent() )
               mfParent()->setModified( true ); // recursive
     } else
          foreach( mfObject * child, mfChildren() ) child->setModified(false);
}


bool mfObject::isModified() const
{
     return m_IsModified;
}

QString mfObject::absoluteFileName() const
{
     if ( !m_AbsoluteFileName.isEmpty() )
          return m_AbsoluteFileName;
     if (mfParent())
          return mfParent()->absoluteFileName();
     return "";
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Working with Params --------------------------------------------
//------------------------------------- Slots for script engine ------------------------------------------
//--------------------------------------------------------------------------------------------------------
QString mfObject::name() const
{
     return param( Param_Name ).toString();
}

QString mfObject::label() const
{
     return param( Param_Label ).toString();
}

QVariant mfObject::value() const
{
     if ( ( m_Type == Type_ShortText ) || ( m_Type == Type_LongText ) || ( m_ValueType == PlugType_String ) )
          return m_TextEditorsContent;

     if ( m_Type == Type_Date || ( m_ValueType == PlugType_Date ) )
          return m_DateTime;

     QStringList tmpList = selectedValues();

     if ( tmpList.size() > 0 )
     {
          if ( tmpList.size() > 1 ) return tmpList; //tmpList.join( ", " );
          else return tmpList.at( 0 );
     }

     return QVariant();
}

QStringList mfObject::values() const
{
     if ( ( m_Type == Type_ShortText ) || ( m_Type == Type_LongText ) || ( m_ValueType == PlugType_String ) )
          return QStringList() << m_TextEditorsContent;

     if ( m_Type == Type_Date || ( m_ValueType == PlugType_Date ) )
          return QStringList() << m_DateTime.toString(Qt::ISODate);

     QStringList strLst;
     foreach (int v, m_SelectedValues)
          strLst << QString::number(v);

     return strLst;
}

bool mfObject::setValues(const QStringList &values)
{
     switch (m_Type)
     {
     case Type_ShortText:
     case Type_LongText:
     case PlugType_String:
     {
          QString newText;
          if (values.count())
               newText = values[0];
          setValue(newText);
     }
     break;
     case Type_Date:
     case PlugType_Date:
     {
          QDateTime dateTime;
          if (values.count())
               dateTime = QDateTime::fromString(values[0], Qt::ISODate);
          if (!dateTime.isValid()) // invalid datetime => not acceptable
               return false;

          setValue(dateTime);
     }
     break;
     default:
     {
          QStringList strPossibleValues = possibleValues();
          QSet<int> selectedValues;
          foreach (const QString &str, values)
          {
               bool ok;
               int n = str.toInt(&ok);
               if (!ok || n >= strPossibleValues.count()) // not a number OR over possible values limit => not acceptable
                    return false;

               selectedValues.insert(n);
          }

          if (m_SelectedValues != selectedValues)
          {
               m_SelectedValues = selectedValues;
               setModified(true);
               emitValueChanged();
          }
     }
     }

     return true;
}

void mfObject::setValue( const QVariant & value )
{
     switch ( m_Type )
     {
          case Type_ShortText:
          case Type_LongText :
               selectedValueChangedTo( value.toString() );
               break;
          case Type_Date :
               selectedValueChangedTo( value.toDateTime() );
               break;
          default :
               if ( m_ValueType != NotDefined )
               {
                    switch ( m_ValueType )
                    {
                         case PlugType_String :
                              selectedValueChangedTo( value.toString() );
                              break;
                         case PlugType_Date :
                              selectedValueChangedTo( value.toDateTime() );
                              break;
                         case NotDefined :
                         case PlugType_StringList :
                              QStringList list = value.toStringList();
                              QStringList posVal = possibleValues();
                              QSet<int> newVals;

                              foreach( const QString &str, list )
                              {
                                   int id = posVal.indexOf( str );
                                   if ( id != -1 ) newVals << id;
                              }
                              selectedValueChangedTo( newVals );
                              break;
                    }
               }
               else
               {
                    QStringList list = value.toStringList();
                    QStringList posVal = possibleValues();
                    QSet<int> newVals;

                    foreach( const QString &str, list )
                    {
                         int id = posVal.indexOf( str );
                         if ( id != -1 ) newVals << id;
                    }
                    selectedValueChangedTo( newVals );
               }
               break;
     }
}


bool mfObject::interpreted() const
{ return m_IsInterpreted; }

void mfObject::setInterpreted( const QVariant & val )
{ m_IsInterpreted = val.toBool(); }

void mfObject::allChildrenInterpreted()
{
     foreach ( mfObject *obj, mfChildren() )
     obj->setInterpreted( true );
     setInterpreted( true );
}

void mfObject::resetInterpreted()
{
     foreach ( mfObject *obj, mfChildren() )
     obj->setInterpreted( false );
     setInterpreted( false );
}


bool mfObject::completed() const
{
     if ( m_IsCompleted ) return true;

     // Check this object
     if ( ( !m_IsModified ) && ( m_IsRequired ) ) return false;

     // Check all children
     foreach ( QObject *obj, children() )
     if ( static_cast<mfObject*>( obj )->completed() )
          return false;
     return true;
}

void mfObject::setCompleted( const QVariant & complete )
{
     m_IsCompleted = complete.toBool();
     foreach ( QObject *obj, children() )
     static_cast<mfObject*>( obj )->setCompleted( complete );
}

void mfObject::setChildrenId( const int & id )
{
     foreach( mfObject * chd, mfChildren() )
     chd->selectedValueChangedTo( id );
}

void mfObject::emitValueChanged()
{
// qWarning() << "Value changed" << name();
     emit valueChanged();
     mfObject * parent = mfParent();
     if ( parent ) parent->emitValueChanged();
}

void mfObject::setInterpretation( const QString &  )
{
     // TODO : setInterpretation

}


QString mfObject::interpretation()
{
     if ( m_IsInterpreted ) return QString::null;
     QString ret = "";
     mfScriptor * scriptor = mfCore::scriptor();

     // If a script is set run it
     QString script = param( Param_Script ).toString();
     if ( script.length() )
     {
          scriptor->setObject( this );
          ret = scriptor->run( Param_Script );
     }
     // If mfObject is a date then return the formatted date
     else if ( m_Type == Type_Date )
     {
          ret.append( param( Param_Label ).toString() + ": " );
          ret.append( m_DateTime.toString( m_DateFormat ) );
     }
     // If mfObject got selectedValues return interpretedValues of the selected
     else if ( m_SelectedValues.count() )
     {
          // Return the interpretValue if exists else return the label of possibleValue
          ret.append( param( Param_Label ).toString() + ": " );
          foreach( int id, m_SelectedValues )
          {
               QStringList list;
               if ( param( Param_InterpretValues ).toStringList().count() )
                    {  list = param( Param_InterpretValues ).toStringList(); }
               else
                    {  list = param( Param_PossibleValues ).toStringList(); }
               ret.append( list.at( id ) + "\n" );
          }
     }

     // Call interpretation of children (checkable groupbox to manage)
     if ( ( m_Type == Type_Group ) && ( !isChecked() ) )
          return ret;

     foreach ( QObject *obj, children() )
     ret.append( static_cast<mfObject*>( obj )->interpretation() );

     return ret;
}

int mfObject::id() const
{
     int toReturn = -1;
     if ( m_SelectedValues.size() ) toReturn = *m_SelectedValues.constBegin();
     return toReturn;
}

QSet<int> mfObject::ids() const
{
     return m_SelectedValues;
}

void mfObject::setId( const QVariant & id )
{
     selectedValueChangedTo( id.toInt() );
}

void mfObject::setIds( const QSet<int> & ids )
{
     selectedValueChangedTo( ids );
}

//--------------------------------------------------------------------------------------------------------
//---------------------------------------- Script SUM ----------------------------------------------------
//--------------------------------------------------------------------------------------------------------
QVariant mfObject::sumOfChildren() const
{
     if ( !m_IsModified ) return 0;

     double sum = 0.0;

     foreach ( mfObject *chd, mfChildren() )
     {
          if ( chd->countSelectedValues() )
          {
               QStringList iValues = chd->param( Param_NumericalValues ).toStringList();
               if ( !iValues.count() ) continue;
               foreach( int id, chd->selectedValuesIds() )
               sum += iValues.at( id ).toDouble() ;
          }
          sum += chd->sumOfChildren().toDouble();
     }

     return sum;
}

QString mfObject::sumOfInterpretValues( const QString & separator ) const
{
     if ( !m_IsModified ) return 0;

     QString sum;

     foreach ( mfObject *chd, mfChildren() )
     {
          if ( chd->countSelectedValues() )
          {
               QStringList iValues = chd->param( Param_InterpretValues ).toStringList();
               if ( !iValues.size() ) return QString::null;
               foreach( int id, chd->selectedValuesIds() )
               {
                    if ( iValues.at( id ).length() )
                         sum += iValues.at( id ) + separator;
               }
          }
          sum += chd->sumOfInterpretValues( separator );
     }

     // The last separator is not needed
     if ( sum.size() )
          sum.chop( separator.length() );

     return sum;
}

QString mfObject::sumOfCheckedLabels( const QString & separator ) const
{
     if ( !m_IsModified ) return 0;

     QString sum;

     foreach( mfObject * chd, mfChildren() )
     {
          if ( chd->type() != Type_Check ) continue;

          if ( chd->isChecked() )
               sum += chd->param( Param_Label ).toString() + separator;

          sum += chd->sumOfCheckedLabels( separator );
     }

     // The last separator is not needed
     if ( sum.size() )
          sum.chop( separator.length() );

     return sum;
}

void mfObject::emitFormUiMustBeUpdated()
{    emit formUiMustBeUpdated();   }


bool mfObject::addFile( const QString & fileName )
{
     mfIOInterface *ioPlugin = mfCore::pluginsManager()->currentIO();
     if (!ioPlugin)
          return false;

     QString fName = fileName;
     QDir d( fileName );
     if ( d.isRelative() )
     {
          QFileInfo fileInfo ( absoluteFileName() );
          fName.prepend( fileInfo.absolutePath() + QDir::separator() );
     }

     mfObject *obj = ioPlugin->loadForm(mfIOPlace::fromLocalFile(fName));
     if ( obj )
          obj->setParent( this );

     // Root Parent emit signal formUiMustBeUpdated
     root()->emitFormUiMustBeUpdated();

     return true;
}
