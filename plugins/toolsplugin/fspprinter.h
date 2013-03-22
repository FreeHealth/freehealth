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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_INTERNAL_FSPPRINTER_H
#define TOOLS_INTERNAL_FSPPRINTER_H

#include <QDate>
#include <QString>

/**
 * \file fspprinter.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 21 Mar 2013
*/

namespace Tools {
namespace Internal {
class FspPrinterPrivate;

struct Fsp_AmountLine {
    Fsp_AmountLine()
        : activity(-1), deplacement_nb(-1), deplacement_amount(0.), depassement(false)
    {}

    QDate date;
    QString act;
    int activity, deplacement_nb;
    QString C, otherAct1, otherAct2, idMd;
    double amount, deplacement_amount;
    bool depassement;
};

struct Fsp_Patient {
    QString fullName, address, nss, nssKey;
    QString assuredNss, assuredNssKey, assuredName, assuranceNumber;
    QDate dob;
};

struct Fsp_Conditions {
    Fsp_Conditions() :
          maladie(0), exonerationTM(0), ald(0), accidentParTiers(0), prevention(0), autre(0), soinL115(0),
          maternite(0), atMP(0), nouveauMT(0),
          accessSpecific(0), urgence(0), horsResidence(0), medTraitantRemplace(0), accessHorsCoordination(0)
    {}

    bool maladie, exonerationTM, ald, accidentParTiers, prevention, autre, soinL115;
    bool maternite, atMP, nouveauMT;
    QString atMPNumber;
    QDate accidentParTiersDate, dateGrossesse, atMPDate;
    QString envoyeParMedecin;
    bool accessSpecific, urgence, horsResidence, medTraitantRemplace, accessHorsCoordination;
    QDate dateAccordPrealable;
};

class FspPrinter
{
public:
    enum Cerfa {
        S12541_01 = 0,
        S12541_02
    };

    explicit FspPrinter();
    ~FspPrinter();

    void setBillNumber(const QString &uid);
    void setBillDate(const QDate &date);

    void setPatient(const Fsp_Patient &patient);
    void setConditions(const Fsp_Conditions &conds);
    void addAmountLine(const Fsp_AmountLine &line);
    void setPayment(bool nAPasPayePartObligatoire, bool nAPasPayePartComplementaire);

    void clear();

    bool print(Cerfa cerfa = S12541_01);

private:
    FspPrinterPrivate *d;
};

} // namespace Internal
} // namespace Tools

#endif // TOOLS_INTERNAL_FSPPRINTER_H

