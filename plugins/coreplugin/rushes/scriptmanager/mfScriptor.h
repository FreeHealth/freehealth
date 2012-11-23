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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MFSCRIPTOR_H
#define MFSCRIPTOR_H

#include <QObject>
#include <mfExporter.h>
class mfObject;

QT_BEGIN_NAMESPACE
class QScriptEngine;
QT_END_NAMESPACE


/**
 * \file mfScriptor.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.04
 * \date 20 octobre 2008
*/


class Q_FMF_EXPORT mfScriptor : public QObject
{
     Q_OBJECT
public:

     //---------------------------------------------------------------------------------------------------
     //------------------------------------ Constructor / Destructor -------------------------------------
     //------------------------------------  Scriptor Initialisators -------------------------------------
     //---------------------------------------------------------------------------------------------------
     mfScriptor( QObject *parent = 0 );
     ~mfScriptor();

     bool setObject( mfObject * mfo );
     QString run( int param );

     //---------------------------------------------------------------------------------------------------
     //---------------------------------------- Finding objects ------------------------------------------
     //---------------------------------------------------------------------------------------------------
public slots:
//      QString get( const QString & item, const QString & askedProperty, mfObject * fromThisObject = 0 ) const;


private:

/*     bool checkOpenersClosers( const mfObject * mfo );
     mfObject * findObjectNamed( mfObject * mfo, const QString & item ) const;
*/

private:

     mfObject * m_Object;
     mfObject * m_RootObject;
     QString    m_ErrorMsg;
     QScriptEngine * m_Engine;
};

#endif
