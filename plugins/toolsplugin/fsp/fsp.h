/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric Maeker                                         *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_INTERNAL_FSP_H
#define TOOLS_INTERNAL_FSP_H

#include <QDate>
#include <QString>
#include <QVariant>
/**
 * \file fsp.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 23 Mar 2013
*/

namespace Tools {
namespace Internal {
class FspPrivate;

class Fsp
{
public:
    enum Data {
        Bill_Number = 0,
        Bill_Date,
        Patient_FullName,
        Patient_FullAddress,
        Patient_DateOfBirth,
        Patient_Personal_NSS,
        Patient_Personal_NSSKey,
        Patient_Assure_FullName,
        Patient_Assure_NSS,
        Patient_Assure_NSSKey,
        Patient_Assurance_Number,
        Condition_Maladie,
        Condition_Maladie_ETM,
        Condition_Maladie_ETM_Ald,
        Condition_Maladie_ETM_Autre,
        Condition_Maladie_ETM_L115,
        Condition_Maladie_ETM_Prevention,
        Condition_Maladie_ETM_AccidentParTiers_Oui,
        Condition_Maladie_ETM_AccidentParTiers_Date,
        Condition_Maternite,
        Condition_Maternite_Date,
        Condition_ATMP,
        Condition_ATMP_Number,
        Condition_ATMP_Date,
        Condition_NouveauMedTraitant,
        Condition_MedecinEnvoyeur,
        Condition_AccesSpecifique,
        Condition_Urgence,
        Condition_HorsResidence,
        Condition_Remplace,
        Condition_HorsCoordination,
        Condition_AccordPrealableDate,
        Unpaid_PartObligatoire,
        Unpaid_PartComplementaire,
        TotalAmount,
        MaxData
    };

    enum AmountData {
        Amount_Date = 100,
        Amount_ActCode,
        Amount_Activity,
        Amount_CV,
        Amount_OtherAct1,
        Amount_OtherAct2,
        Amount_Amount,
        Amount_Depassement,
        Amount_Deplacement_IKMD,
        Amount_Deplacement_Nb,
        Amount_Deplacement_IKValue,
        Amount_MaxData
    };

    enum ModelData {
        Label = 1000,
        ModelData_ColumnCount
    };

    enum XmlData {
        PatientData   = 0x00001,
        BillData      = 0x00002,
        ConditionData = 0x00004,
        UnpaidData    = 0x00008,
        ModelData     = 0x00010,
        AmountData    = 0x01000,
        AllData       = PatientData | BillData | ConditionData | AmountData | ModelData
    };
    Q_DECLARE_FLAGS(XmlDataSet, XmlData)

    Fsp();
    Fsp(const Fsp &cp);
    Fsp &operator=(const Fsp &cp);

    ~Fsp();
    void clear();

    void setData();

    void setBillNumber(const QString &uid);
    void setBillDate(const QDate &date);

    bool populateWithCurrentPatientData();
    void populateAmountsWithCurrentDate();

    bool setData(int index, const QVariant &value);
    QVariant data(int index) const;

    void addAmountData(int line, int index, const QVariant &value);
    QVariant amountLineData(int line, int index) const;

    void computeTotalAmountFromFees();

    QString toXml(XmlDataSet dataSet = AllData) const;
    QList<Fsp> fromXml(const QString &content);
    QList<Fsp> fromXmlFile(const QString &absPathFileName);

private:
    FspPrivate *d;
};

} // namespace Internal
} // namespace Tools

Q_DECLARE_OPERATORS_FOR_FLAGS(Tools::Internal::Fsp::XmlDataSet)

#endif // TOOLS_INTERNAL_FSP_H
