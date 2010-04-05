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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DOSAGEMODEL_H
#define DOSAGEMODEL_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/constants.h>

#include <QObject>
#include <QSqlTableModel>
#include <QSet>
#include <QFlags>

/**
 * \file dosagemodel.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 23 Fev 2010
*/

namespace DrugsDB {
class DrugsModel;

namespace Internal {

class DRUGSBASE_EXPORT DosageModel : public QSqlTableModel
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
    explicit DosageModel(DrugsDB::DrugsModel *parent = 0);

    virtual int columnCount(const QModelIndex & = QModelIndex()) const { return Dosages::Constants::MaxParam; }
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual QVariant data(const QModelIndex & item, int role = Qt::DisplayRole) const;
    virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
    virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
    virtual void revertRow(int row);

    void setTable (const QString &) {}

    virtual bool setDrugUID(const int uid);
    int drugUID();

    QStringList isDosageValid(const int row);
    bool isDirty(const int row) const;

    QString toXml(const int row);
    bool addFromXml(const QString &xml);
    void toPrescription(const int row);

#ifdef FREEDIAMS
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
    void warn(const int row = -1);

private Q_SLOTS:
    void changeEvent(QEvent * event);
    static void retranslate();

    //--------------------------------------------------------------------------------------------------------
    //----------------------------------------- PRIVATE DATAS ------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
private:
    DrugsDB::DrugsModel *m_DrugsModel;
    static QStringList   m_ScoredTabletScheme;
    static QStringList   m_PreDeterminedForms;
    static QString       m_ActualLangage;
    int m_UID;
    QSet<int> m_DirtyRows;
};

}  // End Internal
}  // End DrugsDB

#endif // DOSAGEMODEL_H
