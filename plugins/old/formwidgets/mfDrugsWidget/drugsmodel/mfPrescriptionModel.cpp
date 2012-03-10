/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "mfPrescriptionModel.h"

// include drugs headers
#include "drugsmodel/mfDrugInteraction.h"

// include Qt headers
#include <QIcon>
#include <QApplication>

QVariant mfPrescriptionModel::data( const QModelIndex &index, int role ) const
{
     if ( !index.isValid() )
          return QVariant();

     if (( index.row() > m_DrugsList.count() ) || ( index.row() < 0 ) )
          return QVariant();

     mfDrugs * drug = m_DrugsList.at( index.row() );

     if ( role == Qt::DisplayRole ) {
          QString display = "";
          display.append( drug->denomination() );
          return display;

     }
     else if ( role == Qt::DecorationRole ) {
         return mfDrugInteraction::iamIcon( drug, m_levelOfWarning );
     }
     else if ( role == Qt::ToolTipRole ) {
          QString display;
          display = drug->toHtml();

          QDrugsList withoutThis = m_DrugsList;
          withoutThis.removeOne( drug );
          if ( mfDrugsBase::instance()->drugHaveInteraction( drug ) ) {
               const QList<mfDrugInteraction *> & list = mfDrugsBase::instance()->getInteractions( drug );
               display.append( "<br>\n" );
               display.append( mfDrugInteraction::listToHtml( list, false ) );
          }
          return display;
     }
     return QVariant();
}


Qt::ItemFlags mfPrescriptionModel::flags( const QModelIndex &index ) const
{
     if ( !index.isValid() )
          return Qt::ItemIsEnabled;

     return QAbstractItemModel::flags( index );
}


bool mfPrescriptionModel::removeRows( int row, int count, const QModelIndex & parent )
{
    beginRemoveRows( parent, row, row+count );

    if ( row >= m_DrugsList.count() )
        return false;

    if ( ( row + count ) > m_DrugsList.count() )
        return false;

    int i;
    for( i = 0; i < count; ++i )
        m_DrugsList.removeAt( row );

    // check drugs interactions from mfDrugsBase
     mfDrugsBase::instance()->interactions( m_DrugsList );

    endRemoveRows();
    reset();
    return true;
}


void mfPrescriptionModel::addDrug( mfDrugs* d )
{
     if ( m_DrugsList.empty() )
     {
          if ( !m_DrugsList.contains( d ) )
               m_DrugsList.append( d );
     }
     else
          m_DrugsList <<  d;

     // check drugs interactions from mfDrugsBase
     mfDrugsBase::instance()->interactions( m_DrugsList );
     m_levelOfWarning = qApp->property( "levelOfWarning" ).toInt();
     reset();
}

void mfPrescriptionModel::addDrug( int CIS )
{
    addDrug( mfDrugsBase::instance()->getDrugByCIS( CIS ) );
}

void mfPrescriptionModel::clearDrugsList()
{
     m_DrugsList.clear();
     m_levelOfWarning = qApp->property( "levelOfWarning" ).toInt();
     reset();
}

void mfPrescriptionModel::setDrugsList( QList<mfDrugs*> & list )
{
     m_DrugsList.clear();
     m_DrugsList << list;

     // check drugs interactions from mfDrugsBase
     mfDrugsBase::instance()->interactions( m_DrugsList );
     m_levelOfWarning = qApp->property( "levelOfWarning" ).toInt();
     reset();
}

QString mfPrescriptionModel::toString()
{
    return " this is a test ";
}
