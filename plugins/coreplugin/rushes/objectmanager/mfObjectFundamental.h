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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef MFOBJECTFUNDAMENTAL_H
#define MFOBJECTFUNDAMENTAL_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QVariant>
#include <QDateTime>
#include <QSet>

#include "mfExporter.h"

/**
 * \file mfObjectFundamental.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.04
 * \date 20 octobre 2008
*/


//--------------------------------------------------------------------------------------------------------
//---------------------------------------    Some TyeDefinitions  ----------------------------------------
//--------------------------------------------------------------------------------------------------------
typedef QMap<int, QVariant>       QMapParams;

typedef QMap<QString, QMapParams> QMapTranslatedParams ;


//--------------------------------------------------------------------------------------------------------
//---------------------------------------- Begining of the classe ----------------------------------------
//--------------------------------------------------------------------------------------------------------
class Q_FMF_EXPORT mfObjectFundamental : public QObject
{
     Q_OBJECT

public:

     //---------------------------------------------------------------------------------------------------
     //------------------------------------    Defining Params    ----------------------------------------
     //---------------------------------------------------------------------------------------------------
     enum TypeEnum   { Type_Undefined = 0, Type_Form, Type_Radio, Type_Check, Type_Combo, Type_MultiCheck,
                       Type_UniqueList, Type_MultiList, Type_Spin, Type_ShortText,
                       Type_LongText, Type_HelpText, Type_File, Type_Group, Type_Date,
                       Type_Button, Type_MaxType
                     };

     enum MapParams   { Param_Name = 0, Param_Type, Param_Label, Param_Version,
                        Param_CreationDate, Param_Categorie, Param_PossibleValues, Param_DefaultValue,
                        Param_Options, Param_InterpretValues, Param_NumericalValues,
                        Param_OnLoadScript, Param_Script, Param_Authors, Param_Bibliography, Param_License,
                        Param_MaxType
                      };


     enum Option      { NoOption = 0x0000,    LabelOnTop = 0x0001,   Vertical = 0x0002,
                        Horizontal = 0x0004,  Checkable = 0x0008,    Checked = 0x0016,
                        CompactView = 0x0032, NumericOnly=0x0064,    SizePreferred = 0x0128,
                        BoldLabel = 0x0256,   Hide= 0x0512
                      };
     Q_DECLARE_FLAGS( Options, Option )

     enum FuncSetParam { ReplaceParam = 0, AppendParam, PrependParam };


     enum PlugSelectedValueType { NotDefined=0, PlugType_String, PlugType_StringList,
                                  PlugType_Date, PlugType_Ids
                                };

     static const QStringList typeNames;
     static const QStringList paramList;
     virtual bool operator==( const mfObjectFundamental & o ) const;

     //---------------------------------------------------------------------------------------------------
     //-------------------------------------- Constructor / Destructor -----------------------------------
     //---------------------------------------------------------------------------------------------------
     mfObjectFundamental( mfObjectFundamental *parent );
     virtual ~mfObjectFundamental();

     //---------------------------------------------------------------------------------------------------
     //------------------------------- Working with unspecific params  -----------------------------------
     //---------------------------------------------------------------------------------------------------
     virtual bool        setParam( const int prm, const QVariant & value, const QString & lang = "default",
                                   const int funcSetParam = ReplaceParam );
     virtual bool        setParam( const int prm, const QString & value, const QString & lang = "default",
                                   const int funcSetParam = ReplaceParam );
     virtual bool        setParam( const int prm, const QStringList & value, const QString & lang = "default",
                                   const int funcSetParam = ReplaceParam );
     virtual bool        setParam( const int prm, const char* value, const QString & lang = "default",
                                   const int funcSetParam = ReplaceParam );
     virtual QVariant    param( const int param ) const;
     virtual void        setPluginSelectedValueType( PlugSelectedValueType type );

     virtual bool        analyseOptions( const QString & value );
     virtual Options     options() const { return m_Options; }

     virtual bool        paramIsStringList( const int param ) const;
     static QStringList  getDefaultTypeNames() { return typeNames; }


     const QMapTranslatedParams &translatedParams() const { return m_TranslatedParams; }

     //---------------------------------------------------------------------------------------------------
     //-------------------------------- Working with specific params  ------------------------------------
     //---------------------------------------------------------------------------------------------------
     virtual void        setAbsoluteFileName( const QString & fileName );

     virtual QDateTime   dateTime() const;
     virtual QString     dateFormat() const;
     virtual QString     mask() const { return m_Mask; }

     virtual void        setModified( const QVariant & state ) = 0;
     virtual bool        isModified() const = 0;
     virtual bool        isChecked() const;
     bool                isRequired() const { return m_IsRequired; }

     virtual int         getNumberOfColumn( int defaultNumber = 1 );

     //---------------------------------------------------------------------------------------------------
     //--------------------------------------- Working with type -----------------------------------------
     //---------------------------------------------------------------------------------------------------
     virtual bool        setType( const int t );
     virtual bool        setType( const QString & t );
     virtual int         typeFromString( const QString & t ) const;
     virtual int         type() const { return m_Type; }

     //---------------------------------------------------------------------------------------------------
     //------------------------------------ Possible and Selected values ---------------------------------
     //---------------------------------------------------------------------------------------------------
     virtual void        setPossibleValues( QStringList & values, const QString & lang = "default"  );
     virtual void        addPossibleValue( const QString & value, const QString & lang = "default" );
     virtual int         countPossibleValues() const;
     virtual QString     possibleValue( int id ) const;
     virtual QStringList possibleValues() const;

     //---------------------------------------------------------------------------------------------------
     //------------------------------------ Completion informations --------------------------------------
     //---------------------------------------------------------------------------------------------------
     virtual bool        isNull() const;
     virtual void        setRequired( bool req );

     //---------------------------------------------------------------------------------------------------
     //----------------------------------- Object Interpretation -----------------------------------------
     //---------------------------------------------------------------------------------------------------
     virtual QStringList interpretedValues() const;
     virtual QList<double> numericalValues() const;

     //---------------------------------------------------------------------------------------------------
     //---------------------------------- Signals / Slot connections -------------------------------------
     //---------------------------------------------------------------------------------------------------
public slots:
     virtual QString     absoluteFileName() const = 0;
     virtual QString     name() const = 0;
     virtual QString     label() const = 0;

     //---------------------------------------------------------------------------------------------------
     //---------------------------------- Selected values changer ----------------------------------------
     //---------------------------------------------------------------------------------------------------
     virtual void        selectedValueChangedTo( const QSet<int> & );
     virtual void        selectedValueChangedTo( const int & );
     virtual void        selectedValueChangedTo( const QString &, const bool emitChangeSignal = true );
     virtual void        selectedValueChangedTo( const QDateTime & value );


     virtual bool        isMultiSelection();

     virtual QStringList selectedValues() const;
     virtual int         countSelectedValues() const;
     virtual QSet<int>   selectedValuesIds() const;


signals:
     void valueChanged();
public:
     virtual void emitValueChanged() = 0;

     //---------------------------------------------------------------------------------------------------
     //-------------------------------------- Private datas of object ------------------------------------
     //---------------------------------------------------------------------------------------------------
protected:


     // Object parameters
     int                   m_Type;                   /*!< Type of object \sa TypeEnum */
     bool                  m_IsModified;             /*!< State of modification */
     bool                  m_IsInterpreted;          /*!< State of interpretation */
     QSet<int>             m_SelectedValues;         /*!< Set of selected index in Param_PossibleValue */
     QString               m_TextEditorsContent;     /*!< Content of the text's object */
     QMapParams            m_ParamsMap;              /*!< Map of params,
                                                          QVariant can be a String or a StringList */
     QMapTranslatedParams  m_TranslatedParams;       /*!< Map of m_ParamsMap's translations */
     bool                  m_IsRequired;             /*!< State of completion (required/optional) */
     QString               m_DateFormat;             /*!< Only used if object if a Date */
     QDateTime             m_DateTime;               /*!< Value of object if it is a Date object */
     QString               m_AbsoluteFileName;       /*!< FileName of form (absolute path) */
     QString               m_Mask;                   /*!< Contains the mask passed in option tag */

     Options               m_Options;                /*!< Stores the asked options into QFlags */
     PlugSelectedValueType m_ValueType;              /*!< Stores the type of selected value of the widget
                                                          Essentially used for script value fonction mfObject */
};
Q_DECLARE_OPERATORS_FOR_FLAGS( mfObjectFundamental::Options )
#endif
