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
#ifndef MFOBJECT_H
#define MFOBJECT_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QVariant>

#include "mfObjectFundamental.h"
#include <QDebug>

/**
 * \file mfObject.h
 * \author Eric MAEKER <eric.maeker@free.fr>, Guillaume Denry
 * \version 0.05
 * \date 25 octobre 2008
*/


class Q_FMF_EXPORT mfObject : public mfObjectFundamental
{
     Q_OBJECT
     Q_PROPERTY( QString name    READ name )
     Q_PROPERTY( QString label   READ label )
     Q_PROPERTY( QString interpretation    WRITE setInterpretation     READ interpretation )
     Q_PROPERTY( QVariant value            WRITE setValue              READ value          )
     Q_PROPERTY( QVariant id               WRITE setId                 READ id             )
     Q_PROPERTY( QVariant interpreted      WRITE setInterpreted        READ interpreted    )
     Q_PROPERTY( QVariant completed        WRITE setCompleted          READ completed      )
     Q_PROPERTY( QVariant modified         WRITE setModified           READ isModified     )
     Q_PROPERTY( QSet<int> ids             WRITE setIds                READ ids            )

public:
     mfObject( mfObject *parent = 0 );
     virtual ~mfObject();

     //---------------------------------------------------------------------------------------------------
     //---------------------------------------- Genealogy ------------------------------------------------
     //---------------------------------------------------------------------------------------------------
     mfObject *mfParent() const; //!< just a wrapper on QObject::parent()
     QList<mfObject*> mfChildren() const;
     mfObject *childByName(const QString &childName) const;
     mfObject *root() const;
     void setParent( mfObject *parent );

     /**
      * Set the object considered as modified or not, regarding the value of the parameter
      * \param State the If state is true, all parent are considered as modified, it state is false, all children
      * are considered as not modified.
      */
     virtual void        setModified( const QVariant & state );
     virtual bool        isModified() const;

     //---------------------------------------------------------------------------------------------------
     //----------------------------------- Object Interpretation -----------------------------------------
     //---------------------------------------------------------------------------------------------------
     virtual void        runOnLoadScript( mfObject * mfo = 0 );
     virtual void        runScript();
     virtual void        resetInterpreted();

public slots:
     //---------------------------------------------------------------------------------------------------
     //------------------------------------- For script engine -------------------------------------------
     //---------------------------------------------------------------------------------------------------


     //---------------------------------------------------------------------------------------------------
     //----------------------------------- PROPERTIES OF OBJECTS -----------------------------------------
     //---------------------------------------------------------------------------------------------------
     virtual QString     name() const;
     virtual QString     label() const;
     virtual QVariant    value() const;
     QStringList         values() const; //!< returns the list of selected values in string forms
     /**
      * set a list of value in string forms for selected values
      * @return false if values is not of the expected form
      */
     bool                setValues(const QStringList &values);
     virtual QString     interpretation();
     virtual int         id() const;
     virtual QSet<int>   ids() const;
     virtual bool        interpreted() const;
     virtual bool        completed() const;

     virtual void        setValue( const QVariant & val );
     virtual void        setChildrenId( const int & id );
     virtual void        setInterpretation( const QString & val );
     virtual void        setId( const QVariant & id );
     virtual void        setIds( const QSet<int> & ids );
     virtual void        setInterpreted( const QVariant & val );
     virtual void        allChildrenInterpreted();
     virtual void        setCompleted( const QVariant & complete );

     //---------------------------------------------------------------------------------------------------
     //----------------------------------- FUNCTIONS OF OBJECTS ------------------------------------------
     //---------------------------------------------------------------------------------------------------
     virtual QVariant    sumOfChildren() const;
     virtual QString     sumOfInterpretValues( const QString & separator ) const;
     virtual QString     sumOfCheckedLabels( const QString & separator ) const;

     virtual QString     absoluteFileName() const;
     virtual bool        addFile( const QString & fileName );

     //---------------------------------------------------------------------------------------------------
     //------------------------------------- DEBUGS FUNCTIONS --------------------------------------------
     //---------------------------------------------------------------------------------------------------
     virtual QString     toDebug() const;

signals:
     void formUiMustBeUpdated();


     //---------------------------------------------------------------------------------------------------
     //------------------------------------- Private methods of object -----------------------------------
     //---------------------------------------------------------------------------------------------------
private:
     void emitFormUiMustBeUpdated();
     void emitValueChanged();

     //---------------------------------------------------------------------------------------------------
     //-------------------------------------- Private datas of object ------------------------------------
     //---------------------------------------------------------------------------------------------------
     bool          m_IsCompleted;
};

//--------------------------------------------------------------------------------------------------------
//------------------------------------------- DEBUGGING FUNCTIONS ----------------------------------------
//--------------------------------------------------------------------------------------------------------

#ifndef QT_NO_DEBUG_OUTPUT
QDebug operator<<( QDebug dbg, const mfObject * o );
QDebug operator<<( QDebug dbg, const mfObject & o );
#endif

//--------------------------------------------------------------------------------------------------------
//-------------------------------------------- INLINE FUNCTIONS ------------------------------------------
//--------------------------------------------------------------------------------------------------------

inline mfObject *mfObject::mfParent() const
{
     return static_cast<mfObject*>( QObject::parent() );
}

inline QList<mfObject*> mfObject::mfChildren() const
{
     QList<mfObject*> list;
     for ( int i = 0; i < children().count(); ++i )
     {
          Q_ASSERT_X( qobject_cast<mfObject*>( children()[i] ),
                      "mfObject::mfChildren()",
                      qPrintable( QString( "a child of a mfObject is not a mfObject but of type %1!" ).
                                  arg( children()[i]->metaObject()->className() ) ) );
          list.append( static_cast<mfObject*>( children()[i] ) );
     }
     return list;
}

inline mfObject *mfObject::childByName(const QString &childName) const
{
     for ( int i = 0; i < children().count(); ++i)
     {
          Q_ASSERT_X( qobject_cast<mfObject*>( children()[i] ),
                      "mfObject::childrenByName()",
                      qPrintable( QString( "a child of a mfObject is not a mfObject but of type %1!" ).
                                  arg( children()[i]->metaObject()->className() ) ) );
          mfObject *child = qobject_cast<mfObject*>( children()[i] );
          if (!childName.compare(child->name(), Qt::CaseInsensitive))
               return child;
     }
     return 0;
}

inline mfObject *mfObject::root() const
{
     mfObject *obj = const_cast<mfObject*>( this );
     while ( obj->mfParent() )
          obj = obj->mfParent();
     return obj;
}

inline void mfObject::setParent( mfObject *parent )
{
     QObject::setParent( parent );
}


#endif
