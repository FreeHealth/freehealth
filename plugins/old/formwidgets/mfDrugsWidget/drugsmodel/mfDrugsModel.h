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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MFDRUGSMODEL_H
#define MFDRUGSMODEL_H

// include drugswidget headers
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDosageModel.h>
class mfDrugsIO;
class mfDrugsModelPrivate;
class mfInteractionsManager;

// include Qt headers
#include <QAbstractTableModel>

/**
 * \file mfDrugsModel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.13
 * \date 23 July 2009
*/

class mfDrugsModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class mfDosageModel;
    friend class mfDrugsIO;
public:
    enum PrescriptionDeserializer {
        AddPrescription,
        AddWithoutDouble,
        ReplacePrescription
    };

    mfDrugsModel( QObject * parent = 0 );
    ~mfDrugsModel();

    // MODEL FUNCTIONS
    QModelIndex index( int row, int column, const QModelIndex & drugParent = QModelIndex() ) const;

    // GETTING DATAS
    int rowCount( const QModelIndex & = QModelIndex() ) const;
    int columnCount( const QModelIndex & = QModelIndex() ) const           { return Drug::MaxParam; }
    bool removeRows( int row, int count, const QModelIndex & parent = QModelIndex() );
    bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    QVariant data( const QModelIndex & index, int role ) const;
    QVariant drugData( const int CIS, const int column );
    bool setDrugData( const int CIS, const int column, const QVariant &value);
    void resetModel();
    Qt::ItemFlags flags( const QModelIndex & index ) const;

    bool prescriptionHasInteractions();

    // ADD / REMOVE DRUGS
    void setDrugsList( QDrugsList & list );
    const QDrugsList & drugsList() const;
    void clearDrugsList();
    int addDrug( mfDrugs* drug, bool automaticInteractionChecking = true );
    int addDrug( const int _CIS, bool automaticInteractionChecking = true );
    int removeDrug( const int _CIS );
    int removeLastInsertedDrug();
    bool containsDrug(const int CIS) const;

    // SORT / ORDER DRUGS INTO PRESCRIPTION
    void sort( int column, Qt::SortOrder order = Qt::AscendingOrder );
    bool moveUp( const QModelIndex & item );
    bool moveDown( const QModelIndex & item );

    // FILTERS
    void showTestingDrugs(bool state);
    bool testingDrugsAreVisible() const;

    // FOR DOSAGE MANAGEMENT
    mfDosageModel *dosageModel( const int _CIS );
    mfDosageModel *dosageModel( const QModelIndex & drugIndex );
    mfInteractionsManager *currentInteractionManger() const;

    void warn();

Q_SIGNALS:
    void prescriptionResultChanged( const QString & html );
    void numberOfRowsChanged();

protected:
    QModelIndex createIndex( int row, int column, void * ptr = 0 ) const;
    QModelIndex createIndex( int row, int column, quint32 id ) const;
    virtual void checkInteractions() const;

private:
    mfDrugsModelPrivate *d;
};


#endif
