/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef MFPRINTER_H
#define MFPRINTER_H

#include <mfObject.h>
#include <mfExporter.h>

#include <QObject>

/**
 * \file mfPrinter.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.4
 * \date 07 Jun 2009
*/
class Q_FMF_EXPORT mfPrinter : public QObject
{
    Q_OBJECT
public:

     mfPrinter( mfObject * mfo );
     ~mfPrinter();

     QString toPrintableHtml();
     bool print();
//      QString objectToPrintableHtml();


private:

     enum GraphicChars { RadioChar = 0x2460,// 0x274D,
                         CheckChar = 0x274F,
                         TopLeftCorner = 0x256D,
                         TopRightCorner = 0x256E,
                         BottomLeftCorner =  0x23A3,
                         BottomRightCorner = 0x256F };

     QString createMultiPossibleValues( mfObject * mfo, QChar car );

     void addHtmlObject( mfObject * mfo, QStringList & list );
     void createForm( mfObject * formRoot );
     QString createGroup( mfObject * mfo );
     QString createRadio( mfObject * mfo );
     QString createList( mfObject * mfo );

     QString retrieveFile( const QString & fileName ) const;
     QString prepareLine( mfObject * mfo, QString text );
     QString prepareTable( QStringList & list, int nbCol, int border = 0 );

     mfObject *      m_Object;
     QStringList     m_ToHtml;

};

#endif
