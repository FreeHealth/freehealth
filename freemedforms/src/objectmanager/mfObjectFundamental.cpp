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
#include "mfObjectFundamental.h"
#include <QDir>
#include <QLocale>
#include <QDebug>

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- Understanding Object (consts) -----------------------------------
//--------------------------------------------------------------------------------------------------------
const QStringList mfObjectFundamental::typeNames =
     QStringList() << "undef" << "form" << "radio" << "check" << "combo"
     << "multicheck" << "uniquelist" << "multilist" << "spin"
     << "shorttext" << "longtext" << "helptext" << "file" << "group" << "date" << "button";

const QStringList mfObjectFundamental::paramList =
     QStringList() << "name" << "type" << "label" << "version"
     << "cdate" << "categorie" << "possiblevalues" << "defaultvalue"
     << "option" << "interpretvalues" << "numericalvalues" << "onloadscript" << "script"
     << "authors" << "bibliography" << "license";

// const QStringList mfObjectFundamental::optionList = QStringList() << "checkable" << "ischecked";

//--------------------------------------------------------------------------------------------------------
//---------------------------------------- Creator and destructor ----------------------------------------
//--------------------------------------------------------------------------------------------------------
mfObjectFundamental::mfObjectFundamental( mfObjectFundamental *parent )
          : QObject( parent ), m_Type ( Type_Undefined ), m_IsModified ( false ), m_IsInterpreted ( false ),
          m_TextEditorsContent ( "" ), m_IsRequired( true ), m_DateFormat( "yyyy-MM-dd" )
{
     // Initialise the known params list
     int max = paramList.count();
     int i = 0;
     for ( i = 0; i < max; ++i )
     {
          m_ParamsMap.insert( i, QVariant() );
     }

     // Intialize all object datas to the defaut values
     m_SelectedValues.clear();
     m_DateTime = QDateTime::currentDateTime();
     m_Options = 0;
     m_ValueType = NotDefined;
}


mfObjectFundamental::~mfObjectFundamental()
{
}

bool mfObjectFundamental::operator==( const mfObjectFundamental & o ) const
{
qWarning() << this->name() << "==" << o.name();
     // compare parameters
     if ( this->name() != o.name() ) return false;
     if ( this->label() != o.label() ) return false;
     if ( this->type() != o.type() ) return false;
     if ( this->options() != o.options() ) return false;
     if ( this->countPossibleValues() != o.countPossibleValues() ) return false;
     if ( this->possibleValues() != o.possibleValues() ) return false;

     // if objects are the same, compare values
     if ( this->dateTime() != o.dateTime() ) return false;
     if ( this->dateFormat() != o.dateFormat() ) return false;
     if ( this->mask() != o.mask() ) return false;

     if ( this->isModified() != o.isModified() ) return false;
     if ( this->isChecked() != o.isChecked() ) return false;
     if ( this->isRequired() != o.isRequired() ) return false;
     return true;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------ Working with unspecific Params ------------------------------------
//--------------------------------------------------------------------------------------------------------
bool mfObjectFundamental::paramIsStringList( const int param ) const
{
     return ( ( param == Param_PossibleValues ) || ( param == Param_InterpretValues ) ||
              ( param == Param_NumericalValues ) || ( param == Param_Options ) ||
              ( param == Param_Bibliography ) || ( param == Param_Authors ) );
}

void mfObjectFundamental::setPluginSelectedValueType( PlugSelectedValueType type )
{
     m_ValueType = type;
}

bool mfObjectFundamental::setParam( const int prm, const QVariant & value,
                                    const QString & lang, const int funcSetParam )
{
     // If lang is not defined, define it to the actual QLocale name
     QString language = lang;
     if ( lang.isEmpty() ) language = QLocale().name().left( 2 );

     QMapParams &paramsMap = m_TranslatedParams[language];

     // If param to manage is "type" so check if passed value is a reconized type
     if ( prm == Param_Type )
     {
          if ( typeNames.contains( value.toString() ) )
          {
               int t = typeNames.indexOf( value.toString() );
               setType( t );
          }
          else
               setType( Type_Undefined );
     }
     else if ( prm == Param_Name )
          this->setObjectName( value.toString() );

     // Manage known options, store unknown options
     if ( prm == Param_Options )
     {
          QStringList actualOptions = paramsMap[prm].toStringList();
          if ( value.toString().contains( ";" ) )
          {
               QStringList list = value.toString().split( ";" );
               foreach( QString op, list )
               {
                    if ( !analyseOptions( op.trimmed() ) ) actualOptions.append( op.trimmed() );
               }
          }
          else
          {
               if ( !analyseOptions( value.toString() ) )
                    actualOptions.append( value.toString().trimmed() );
          }
          paramsMap[prm] = actualOptions;
     }
     else
     // If param is a list of string, can do : append, prepend or replace. funcSetParam says what do to.
     if ( paramIsStringList( prm ) )
     {
          switch ( funcSetParam )
          {
               case AppendParam:
               {
                    QStringList l = paramsMap[prm].toStringList();
                    l.append( value.toString() );
                    paramsMap[prm] = l;
                    break;
               }
               case PrependParam:
               {
                    QStringList l = paramsMap[prm].toStringList();
                    l.prepend( value.toString() );
                    paramsMap[prm] = l;
                    break;
               }
               case ReplaceParam :
                    paramsMap[prm] = value;
                    break;
          }
     }
     else
          { paramsMap[prm] = value; }

     return true;
}

bool mfObjectFundamental::analyseOptions( const QString & value )
{
     bool isKnownOption = true;
     if ( value.startsWith( "format=" ) )
          m_DateFormat = value.mid( value.indexOf( "=" ) + 1 );
     else if ( value.startsWith( "mask=" ) )
          m_Mask = value.mid( value.indexOf( "=" ) + 1 );
     else if ( value.compare( "labelontop", Qt::CaseInsensitive ) == 0 )
          m_Options |= LabelOnTop;
     else if ( value.compare( "vertical", Qt::CaseInsensitive ) == 0 )
          m_Options |= Vertical;
     else if ( value.compare( "horizontal", Qt::CaseInsensitive ) == 0 )
          m_Options |= Horizontal;
     else if ( value.compare( "checkable", Qt::CaseInsensitive ) == 0 )
          m_Options |= Checkable;
     else if ( value.compare( "checked", Qt::CaseInsensitive ) == 0 )
          m_Options |= Checked;
     else if ( value.compare( "compactview", Qt::CaseInsensitive ) == 0 )
          m_Options |= CompactView;
     else if ( value.compare( "numericonly", Qt::CaseInsensitive ) == 0 )
          m_Options |= NumericOnly;
     else if ( value.compare( "sizepreferred", Qt::CaseInsensitive ) == 0 )
          m_Options |= SizePreferred;
     else if ( value.compare( "boldlabel", Qt::CaseInsensitive ) == 0 )
          m_Options |= BoldLabel;
     else if ( value.compare( "labelbold", Qt::CaseInsensitive ) == 0 )
          m_Options |= BoldLabel;
     else if ( value.compare( "hide", Qt::CaseInsensitive ) == 0 )
          m_Options |= Hide;
     else isKnownOption = false;

     return isKnownOption;
}

bool mfObjectFundamental::setParam( const int prm, const QString & value,
                                    const QString & lang, const int funcSetParam )
{
     QVariant qvar = QVariant( value );
     return setParam( prm, qvar, lang, funcSetParam );
}

bool mfObjectFundamental::setParam( const int prm, const QStringList & value,
                                    const QString & lang, const int funcSetParam )
{
     QVariant qvar = QVariant( value );
     return setParam( prm, qvar, lang, funcSetParam );
}

bool mfObjectFundamental::setParam( const int prm, const char* value,
                                    const QString & lang, const int funcSetParam )
{
     QVariant tmp = QString( value );
     this->setParam( prm, tmp, lang, funcSetParam );
     return true;
}

QVariant mfObjectFundamental::param( const int param ) const
{
     // If lang is not defined, define it to the default value
     QString language = QLocale().name().left(2);
     bool languageIsKnown = m_TranslatedParams.contains( language );

     // If language is known, and param found -> then return the translated param
     if ( ( languageIsKnown ) && ( m_TranslatedParams.value( language ).contains( param ) ) )
          return m_TranslatedParams.value( language ).value( param );

//      // Try "default" language
//      if ( m_TranslatedParams.value( "default" ).contains( param ) )
//           return m_TranslatedParams.value( "default" ).value( param );

     // Return the first known param translation
     foreach( const QString & lng, m_TranslatedParams.keys() )
     {
          if ( m_TranslatedParams.value( lng ).contains( param ) )
               return m_TranslatedParams.value( lng ).value( param );
     }

     // Param does not exist (this is not an error)
     return QVariant();
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------- Working with specific Params -------------------------------------
//--------------------------------------------------------------------------------------------------------
void mfObjectFundamental::setAbsoluteFileName( const QString & fileName )
{
     Q_ASSERT_X( QDir( fileName ).isAbsolute(), "mfObjectFundamental::setAbsoluteFileName : Wrong fileName",
                 qPrintable( QString( "Passing a relative fileName (%1) to function." ).arg( fileName ) ) );
     m_AbsoluteFileName = QDir::cleanPath( fileName );
}


QDateTime mfObjectFundamental::dateTime() const
{
     return m_DateTime;
}

QString mfObjectFundamental::dateFormat() const
{
     return m_DateFormat;
}

bool mfObjectFundamental::isChecked() const
{
     // Because check have only two possiblevalues : unckecked(0) and check(1)
     // Just return the value of selectedvalues[0] (that is 0 or 1)
     Q_ASSERT_X( ( m_Type != Type_Group ) || ( m_Type != Type_Check ), "mfObjectFundamental",
                 qPrintable( QString( "trying to access isChecked() function "
                                      "with a wrong type of mfObject."
                                      " Type is \'%1\', mfObject->name() == %2" )
                             .arg( typeNames[ m_Type ], objectName() ) ) );
     if ( m_SelectedValues.count() )
          return *m_SelectedValues.constBegin();
     return false;
}

int mfObjectFundamental::getNumberOfColumn( int defaultNumber )
{
     int numberColumns = 0;
     QStringList ncol = param( Param_Options ).toStringList();
     ncol = ncol.filter( "col=", Qt::CaseInsensitive );
     if ( ncol.count() )
     {
          QString option = ncol.at( 0 );
          option = option.right( option.indexOf( "=" ) - 2 );
          numberColumns = option.toInt();
     }
     if ( numberColumns <= 0 ) numberColumns = defaultNumber;
     else if ( numberColumns > 10 ) numberColumns = 10;
     return numberColumns;
}

//--------------------------------------------------------------------------------------------------------
//-------------------------------------------- Working this types ----------------------------------------
//--------------------------------------------------------------------------------------------------------
bool mfObjectFundamental::setType( int t )
{
     // This function is reserved for BaseWidgets only plugin must not use it !
     // Plugin type == Type_Undefined
     bool ret = true;
     if ( t < Type_MaxType )
          m_Type = t;
     else
     {
          m_Type = Type_Undefined;
          ret = false;
     }

     // If object is a checkbox add possibleValues "checked" / "unchecked"
     if ( m_Type == Type_Check )
     {
          setParam ( Param_PossibleValues, QStringList() << "unchecked" << "checked" );
     }

     return ret;
}

bool mfObjectFundamental::setType( const QString & t )
{
     int tp = typeNames.indexOf( t );
     if ( ( tp == -1 ) && ( tp > Type_MaxType ) )
          tp = Type_Undefined;
     return this->setType( tp );
}

int mfObjectFundamental::typeFromString( const QString & t ) const
{
     int tp = typeNames.indexOf( t );
     if ( tp == -1 || tp > Type_MaxType )
          return Type_Undefined;
     return tp;
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Completion informations ----------------------------------------
//--------------------------------------------------------------------------------------------------------
bool mfObjectFundamental::isNull() const
{
     // If no params are set --> return false
     foreach ( const QVariant &v, m_ParamsMap )
     {
          if ( !v.isNull() )
               return false;
     }
     return true;
}

void mfObjectFundamental::setRequired( bool req )
{
     m_IsRequired = req;
}


bool mfObjectFundamental::isMultiSelection()
{
     // From the type of object detects multiselection object
     switch ( this->m_Type )
     {
          case Type_MultiList:
          case Type_MultiCheck:
               return true;
          default:
               return false;
     }
}

//--------------------------------------------------------------------------------------------------------
//----------------------------------- Possible and Selected Values ---------------------------------------
//--------------------------------------------------------------------------------------------------------
void mfObjectFundamental::setPossibleValues( QStringList & values, const QString & lang )
{
     setParam( Param_PossibleValues, values, lang );
}

void mfObjectFundamental::addPossibleValue( const QString & value, const QString & lang )
{
     setParam( Param_PossibleValues, value, lang, AppendParam );
}


int mfObjectFundamental::countPossibleValues() const
{ return param( Param_PossibleValues ).toStringList().count(); }


QString mfObjectFundamental::possibleValue( int id ) const
{
     if ( id < countPossibleValues() )
          return param( Param_PossibleValues ).toStringList().at( id );
     return QString::null;
}

QStringList mfObjectFundamental::possibleValues() const
{
     return param( Param_PossibleValues ).toStringList();
}


int mfObjectFundamental::countSelectedValues() const
{
     return m_SelectedValues.size();
}

QStringList mfObjectFundamental::selectedValues() const
{
     if ( !m_IsModified ) return QStringList();

     if ( m_SelectedValues.size() )
     {
          QStringList list;
          foreach( int id, m_SelectedValues )
          {
               list.append( possibleValue( id ) );
          }
          return list;
     }
     else if ( m_TextEditorsContent.size() ) return QStringList() << m_TextEditorsContent;

     return QStringList();
}

QSet<int> mfObjectFundamental::selectedValuesIds() const
{
     return m_SelectedValues;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------- Signals / Slot connections ---------------------------------------
//--------------------------------------------------------------------------------------------------------
void mfObjectFundamental::selectedValueChangedTo( const QSet<int> & set )
{
     // Check if the passed values and the same than the actual values
     if ( m_SelectedValues.count() != 0 &&
          m_SelectedValues == set)
          return;

     m_SelectedValues = set;
     setModified( true );
     emitValueChanged();
}

void mfObjectFundamental::selectedValueChangedTo( const int & value )
{
     int tmp = -1;
     if ( m_SelectedValues.count() )
          tmp = *m_SelectedValues.constBegin();
     if ( tmp == value ) return;

     m_SelectedValues.clear();
     m_SelectedValues << value;
     setModified( true );
     emitValueChanged();
}


void mfObjectFundamental::selectedValueChangedTo( const QDateTime & value )
{
     if ( value == m_DateTime ) return;
     m_SelectedValues.clear();
     m_DateTime = value;
     setModified( true );
     emitValueChanged();
}

void mfObjectFundamental::selectedValueChangedTo( const QString & value, const bool emitChangeSignal )
{
     m_SelectedValues.clear();
     int id = param( Param_PossibleValues ).toStringList().indexOf( value );
     bool okSignal = true;

     // If the string belongs to the possiblevalue list then add the index of the string
     // else selectedvalues is cleared
     //      add the complete string (for texteditors) is stored in m_TextEditorsContent
     if ( id == -1 )
     {
          // If there's no difference between value and actualValue pass and do not emit signal
          if ( m_TextEditorsContent.compare( value ) != 0 )
               m_TextEditorsContent = value;
          else
               okSignal = false;
     }
     else { m_SelectedValues << id; }

     if ( okSignal )
     {
          setModified( true );
          if ( emitChangeSignal ) emitValueChanged();
     }
}



QStringList mfObjectFundamental::interpretedValues() const
{
     if ( !m_IsModified ) return QStringList();

     if ( m_SelectedValues.size() )
     {
          QStringList list;
          QStringList iValues = param( Param_InterpretValues ).toStringList();
          foreach( int id, m_SelectedValues )
          list << iValues[id];
          return list;
     }
     else if ( m_TextEditorsContent.size() )
          return QStringList() << m_TextEditorsContent;

     return QStringList();
}


QList<double> mfObjectFundamental::numericalValues() const
{
     if ( !m_IsModified ) return QList<double>();
     QList<double> list;
     if ( m_SelectedValues.size() )
     {
          QStringList iValues = param( Param_NumericalValues ).toStringList();
          foreach( int id, m_SelectedValues )
          list << iValues[id].toDouble();
     }
     return list;
}
