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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef GIRSCORE_H
#define GIRSCORE_H

#include <medicalutils/medical_exporter.h>

#include <QString>

namespace MedicalUtils {
namespace AGGIR {
class OldGirScorePrivate;
class NewGirScorePrivate;

class MEDICALUTILS_EXPORT NewGirScore
{
public:
    enum Item {
        NoItemDefined = 0,
        Transferts,
        DeplacementsInterieurs,
        Toilette,
        Elimination,
        Habillage,
        Alimentation,
        Orientation,
        Coherence,  // 8
        Cuisine,
        Traitement,
        Menage,
        Alerter,
        DeplacementsExterieurs,
        Transports,
        TempsLibre,
        Achats,
        Gestion
    };
    enum SubItem {
        NoSubItem = 0,
        Haut,
        Moyen,
        Bas,
        Urinaire,
        Fecale,
        SeServir,
        Manger,
        Temps,
        Espace,
        Comportement,
        Communication  // 11
    };
    enum Reponse {
        AucuneReponse   = 0x0000,
        NeFaitPas       = 0x0001,
        Spontanement    = 0x0002,
        Totalement      = 0x0004,
        Habituellement  = 0x0008,
        Correctement    = 0x0010,
        AucunProbleme   = 0x1000
    };
    Q_DECLARE_FLAGS(Reponses, Reponse)

    NewGirScore();
    ~NewGirScore();

    bool isNull() const;
    bool isValid() const;
    bool isComplete() const;

    void clear();

    // Variables
    void setValue(Item item, SubItem subItem, const Reponses &reponses);
    void setValue(Item item, const Reponses &reponses);
    Reponses reponses(Item item, SubItem subItem = NoSubItem) const;

    QString getCodeGir(Item item) const;
    QString getCodeGir(Item item, SubItem subItem) const;

    int resultingGir() const;
    QString explanations(int girScore) const;

    QString serializeScore() const;
    bool setSerializedScore(const QString &score) const;

private:
    NewGirScorePrivate *d;
};

class MEDICALUTILS_EXPORT OldGirScore
{
public:

    OldGirScore();
    ~OldGirScore();

    enum chaineGIR {
        GIR_coherence = 0,
        GIR_orientation,
        GIR_toilette,
        GIR_habillage,
        GIR_alimentation,
        GIR_elimination,
        GIR_transferts,
        GIR_interieur
    };

    bool isNull() const;
    bool isValid() const;

    // Variables
    void setValues(const QString &coherence,
                   const QString &orientation,
                   const QString &toilette,
                   const QString &habillage,
                   const QString &alimentation,
                   const QString &elimination,
                   const QString &transferts,
                   const QString &interieur);

    void setCoherence(const QString &val);
    void setOrientation(const QString &val);
    void setToilette(const QString &val);
    void setHabillage(const QString &val);
    void setAlimentation(const QString &val);
    void setElimination(const QString &val);
    void setTransferts(const QString &val);
    void setInterieur(const QString &val);

    // Descriptives
    void setExterieur(const QString &val);
    void setCommunication(const QString &val);

    QString coherence() const;
    QString orientation() const;
    QString toilette() const;
    QString habillage() const;
    QString alimentation() const;
    QString elimination() const;
    QString transferts() const;
    QString interieur() const;
    QString exterieur() const;
    QString communication() const;

    int resultingGir() const;
    QString explanations(int girScore) const;

//protected:
    QString getGirString() const;

private:
    OldGirScorePrivate *d;
};

} // End AGGIR
} // End MedicalUtils

Q_DECLARE_OPERATORS_FOR_FLAGS(MedicalUtils::AGGIR::NewGirScore::Reponses)

#endif  // End GIRSCORE_H

