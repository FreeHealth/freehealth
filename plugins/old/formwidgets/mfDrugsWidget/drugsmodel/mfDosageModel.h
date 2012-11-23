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
#ifndef MFDOSAGEMODEL_H
#define MFDOSAGEMODEL_H

#include <mfDrugsConstants.h>

#include <QObject>
#include <QSqlTableModel>
#include <QSet>
#include <QFlags>

using namespace mfDosagesConstants;

class mfDosageModel : public QSqlTableModel
{
    Q_OBJECT

    enum ScoredTablet
    {
        CompletTablet = 0,
        HalfTablet,
        QuaterTablet
    };

    enum PreDeterminedForms
    {
        DosePerKilo=0,
        SpoonReference,     // cuillère-mesure
        Spoon2_5ml,
        Spoon5ml,
        Puffs,              // bouffées
        Dose,
        Mouthwash,          // bain de bouche
        Inhalation,
        Application,
        Washing,            // lavement
        EyeWash,            // lavage oculaire
        Instillation
    };

public:
    mfDosageModel( QObject *parent = 0 );

    virtual int columnCount( const QModelIndex & = QModelIndex() ) const { return Dosage::MaxParam; }
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    virtual QVariant data( const QModelIndex & item, int role = Qt::DisplayRole ) const;
    virtual bool insertRows( int row, int count, const QModelIndex & parent = QModelIndex() );
    virtual bool removeRows( int row, int count, const QModelIndex & parent = QModelIndex() );
    virtual void revertRow( int row );

    void setTable ( const QString & ) {}

    virtual bool setDrugCIS( const int _CIS );
    int drugCIS();

    QStringList isDosageValid( const int row );
    bool isDirty(const int row) const;

    QString toXml(const int row);
    bool addFromXml(const QString &xml);
    void toPrescription(const int row);

#ifdef DRUGS_INTERACTIONS_STANDALONE
    bool userCanRead()   { return true; }
    bool userCanWrite()  { return true; }
#else
    bool userCanRead();
    bool userCanWrite();
#endif

public Q_SLOTS:
    bool submitAll();

public:
    //--------------------------------------------------------------------------------------------------------
    //---------------------------------------- STATIC MEMBERS ------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
    // static viewers to use for ui generation
    static void         initStaticDatas()      { retranslate() ; }
    static int          periodDefault()        { return 4; }
    static QStringList  scoredTabletScheme();
    static QStringList  predeterminedForms();

    // non static viewers for ui generation (need to be instanciate first)
    QStringList        forms();

    // Debugging Informations
    void warn( const int row = -1 );

private Q_SLOTS:
    void changeEvent( QEvent * event );
    static void retranslate();

    //--------------------------------------------------------------------------------------------------------
    //----------------------------------------- PRIVATE DATAS ------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
private:
    static QStringList    m_ScoredTabletScheme;
    static QStringList    m_PreDeterminedForms;
    static QString        m_ActualLangage;
    int m_CIS;
    QSet<int> m_DirtyRows;
};

#endif // MFDOSAGEMODEL_H
