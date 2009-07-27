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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TKSTRINGLISTMODEL_H
#define TKSTRINGLISTMODEL_H

#include <tkExporter.h>

class tkStringListModelPrivate;

#include <QAbstractListModel>
#include <QObject>
class QStringList;

/**
 * \file tkStringListModel.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.4
 * \date 25 April 2009
*/

class Q_TK_EXPORT tkStringListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /** */
    tkStringListModel( QObject *parent = 0,
                       const bool stringEditable = false,
                       const bool checkStateEditable = true );
    /** */
    ~tkStringListModel();

    /** */
    virtual int rowCount( const QModelIndex & = QModelIndex() ) const;
    /** */
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

    /** */
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    /** */
    virtual QVariant data( const QModelIndex & index, int role ) const;
    /** */
    virtual bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    /** */
    virtual bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );

    /** */
    void setCheckable( bool state );
    /** */
    void setStringEditable( bool state );
    /** */
    void setStringList( const QStringList & strings );
    /** */
    QStringList getStringList() const;

    /** */
    QStringList getCheckedItems() const;
    /** */
    void setCheckedItems( const QStringList & list );

    /** */
    bool moveUp( const QModelIndex & item );
    /** */
    bool moveDown( const QModelIndex & item );

private:
    tkStringListModelPrivate *d;
};

#endif // TKSTRINGLISTMODEL_H
