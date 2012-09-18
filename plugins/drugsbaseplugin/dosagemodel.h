/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
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
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 23 Sept 2010
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

    void setTable(const QString &) {}

    virtual bool setDrugId(const QVariant &drugId);
    QVariant drugId();

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
    static void         initStaticData()      { retranslate() ; }
    static int          periodDefault()        { return 4; }
    static QStringList  scoredTabletScheme();
    static QStringList  predeterminedForms();

    // non static viewers for ui generation (need to be instanciate first)
    QStringList        forms();

    // Debugging Information
    void warn(const int row = -1);

private Q_SLOTS:
    void changeEvent(QEvent * event);
    static void retranslate();

    //--------------------------------------------------------------------------------------------------------
    //----------------------------------------- PRIVATE DATA -------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
private:
    DrugsDB::DrugsModel *m_DrugsModel;
    static QStringList   m_ScoredTabletScheme;
    static QStringList   m_PreDeterminedForms;
    static QString       m_ActualLangage;
    QVariant m_UID;
    QSet<int> m_DirtyRows;
    QSet<int> m_DirtyInnLkRows;
    QHash<int, int> _refills;
    mutable QString m_Route;
};

}  // End Internal
}  // End DrugsDB

#endif // DOSAGEMODEL_H
