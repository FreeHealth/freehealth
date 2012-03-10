/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MFPRESCRIPTIONMODEL_H
#define MFPRESCRIPTIONMODEL_H

// include drugswidget headers
#include "drugsdatabase/mfDrugsBase.h"
#include "drugsmodel/mfDrugs.h"

// include Qt headers
#include <QAbstractListModel>


/**
  \brief The prescription model.

  Model asks mfDrugsBase to check interaction only while passing new *mfDrugs via addDrugs()
  or while passing QDrugsList via setDrugsList().

  This class owns the drugs list and manage memory cleaning.
*/


class mfPrescriptionModel : public QAbstractListModel
{
     Q_OBJECT

public:
    mfPrescriptionModel( QObject * parent = 0 )
               : QAbstractListModel( parent )
    {
        if ( !mfDrugsBase::isInitialized() )
            return;
        m_DrugsList.clear();
    }

    ~mfPrescriptionModel() { qDeleteAll( m_DrugsList ); }

    int rowCount( const QModelIndex & /*parent*/ = QModelIndex() ) const { return m_DrugsList.count(); }
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    QVariant data( const QModelIndex & index, int role ) const;
    Qt::ItemFlags flags( const QModelIndex & index ) const;

    // populate with drugs
    void setDrugsList( QList<mfDrugs*> & list );
    const QList<mfDrugs*> & drugsList() { return m_DrugsList; }
    void addDrug( mfDrugs* );
    void addDrug( int CIS );

    // clearing drugs
    void clearDrugsList();

    // getters
    QString toString();

private:
    QList<mfDrugs*>  m_DrugsList;
    int m_levelOfWarning;
};


#endif  // MFPRESCRIPTIONMODEL_H
