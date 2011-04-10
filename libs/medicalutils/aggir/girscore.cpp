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
 ***************************************************************************/
#include "girscore.h"

#include <QRegExp>
#include <QStringList>


namespace MedicalUtils {
namespace AGGIR {

class GirScorePrivate
{
public:
    static QStringList m_GirExplanations;
    void needNewValidityTesting()
    {
        m_testValidity=true;
        m_valid=false;
    }

    QString m_coherence,
    m_orientation,
    m_toilette,
    m_habillage,
    m_alimentation,
    m_elimination,
    m_transferts,
    m_interieur,
    m_exterieur,
    m_communication;
    bool m_testValidity, m_valid;
};

QStringList GirScorePrivate::m_GirExplanations = QStringList()
        << QString("Le groupe iso-ressources 1 comprend les personnes âgées confinées au lit ou au fauteuil, "
                   "dont les fonctions mentales sont gravement altérées et qui nécessitent une présence "
                   "indispensable et continue d'intervenants.")
        << QString("le groupe iso-ressources 2 concerne les personnes âgées confinées au lit ou au fauteuil, "
                   "dont les fonctions intellectuelles ne sont pas totalement altérées et dont l'état exige une prise en "
                   "charge pour la plupart des activités de la vie courante. Ce groupe s'adresse aussi aux personnes "
                   "âgées dont les fonctions mentales sont altérées, mais qui ont conservé leurs capacités de se déplacer.")
        << QString("Le groupe iso-ressources 2 concerne les personnes âgées confinées au lit ou au fauteuil, "
                   "dont les fonctions intellectuelles ne sont pas totalement altérées et dont l'état exige une prise en "
                   "charge pour la plupart des activités de la vie courante. Ce groupe s'adresse aussi aux personnes "
                   "âgées dont les fonctions mentales sont altérées, mais qui ont conservé leurs capacités de se déplacer. ")
        << QString("Le groupe iso-ressources 4 intègre les personnes âgées n'assumant pas seules leurs "
                   "transferts mais qui, une fois levées, peuvent se déplacer à l'intérieur de leur logement. Elles doivent "
                   "parfois être aidées pour la toilette et l'habillage. Ce groupe s'adresse également aux personnes âgées "
                   "n'ayant pas de problèmes locomoteurs mais devant être aidées pour les activités corporelles et pour les repas.")
        << QString("Le groupe iso-ressources 5 comporte des personnes âgées ayant seulement besoin d'une aide"
                   "ponctuelle pour la toilette, la préparation des repas et le ménage. ")
        << QString("Le groupe iso-ressources 6 réunit les personnes âgées n'ayant pas perdu leur autonomie"
                   "pour les actes essentiels de la vie courante.")
        ;

}
}

using namespace MedicalUtils::AGGIR;


static int getRang(const int rang)
{  int GIR_calcule;
    if (rang == 1){ GIR_calcule = 1; }
    if ((rang >= 2) && (rang <= 7)) { GIR_calcule = 2; }
    if ((rang >= 8) && (rang <= 9)) { GIR_calcule = 3; }
    if ((rang >= 10) && (rang <= 11)) { GIR_calcule = 4; }
    if (rang == 12) { GIR_calcule = 5; }
    if (rang == 13) { GIR_calcule = 6; }

    return GIR_calcule;
}


/*! \brief Calcul le rang correspondant au données de la grille GIR préalablement formatées.
 *  \return le groupe GIR.
 *  \param UneChaine = chaine de caractère contenant l'encodage exacte des items de la GIR. Cette varaible est construite par la fonction : getChaineGIR.
 */
static int getGIR(const QString &chaine)
{
    int rang;
    int groupe;
    if (chaine.length() != 8)
        return 0;

    QString chaineGIR = chaine;

    rang = 0;

    // Teste UneChaine pour le groupe A
    groupe = 0;
    if (chaineGIR[0] == 'C') { groupe = 2000; }
    if (chaineGIR[1] == 'C') { groupe = groupe + 1200; }
    if (chaineGIR[2] == 'C') { groupe = groupe + 40; }
    if (chaineGIR[3] == 'C') { groupe = groupe + 40; }
    if (chaineGIR[4] == 'C') { groupe = groupe + 60; }
    if (chaineGIR[5] == 'C') { groupe = groupe + 100; }
    if (chaineGIR[6] == 'C') { groupe = groupe + 800; }
    if (chaineGIR[7] == 'C') { groupe = groupe + 200; }
    if (chaineGIR[2] == 'B') { groupe = groupe + 16; }
    if (chaineGIR[3] == 'B') { groupe = groupe + 16; }
    if (chaineGIR[4] == 'B') { groupe = groupe + 20; }
    if (chaineGIR[5] == 'B') { groupe = groupe + 16; }
    if (chaineGIR[6] == 'B') { groupe = groupe + 120; }
    if (chaineGIR[7] == 'B') { groupe = groupe + 32; }
    // Rang groupe A
    if (groupe >= 4380 )
    { rang = 1; }
    else
        if ((groupe >= 4140 ) && (groupe <= 4379))
        { rang = 2 ; }
    else
        if ((groupe >= 3390 ) && (groupe <= 4139))
        {  rang = 1 ;}

    if (rang != 0)
    {  return getRang(rang);
    }

    // Teste UneChaine pour le groupe B
    groupe = 0;
    if (chaineGIR[0] == 'C') { groupe = 1500; }
    if (chaineGIR[1] == 'C') { groupe = groupe + 1200; }
    if (chaineGIR[2] == 'C') { groupe = groupe + 40; }
    if (chaineGIR[3] == 'C') { groupe = groupe + 40; }
    if (chaineGIR[4] == 'C') { groupe = groupe + 60; }
    if (chaineGIR[5] == 'C') { groupe = groupe + 100; }
    if (chaineGIR[6] == 'C') { groupe = groupe + 800;  }
    if (chaineGIR[7] == 'C') { groupe = groupe - 80; }
    if (chaineGIR[0] == 'B') { groupe = groupe + 320; }
    if (chaineGIR[1] == 'B') { groupe = groupe + 120; }
    if (chaineGIR[2] == 'B') { groupe = groupe + 16; }
    if (chaineGIR[3] == 'B') { groupe = groupe + 16; }
    if (chaineGIR[4] == 'B') { groupe = groupe + 0; }
    if (chaineGIR[5] == 'B') { groupe = groupe + 16; }
    if (chaineGIR[6] == 'B') { groupe = groupe + 120; }
    if (chaineGIR[7] == 'B') { groupe = groupe - 40; }
    // ' Rang groupe B
    if (groupe >= 2016)
    { rang = 4;
        return getRang(rang);
    }
    else  { rang = 0; }

    // Teste UneChaine pour le groupe C
    groupe = 0;
    if (chaineGIR[2] == 'C') { groupe = groupe + 40; }
    if (chaineGIR[3] == 'C') { groupe = groupe + 40; }
    if (chaineGIR[4] == 'C') { groupe = groupe + 60; }
    if (chaineGIR[5] == 'C') { groupe = groupe + 160; }
    if (chaineGIR[6] == 'C') { groupe = groupe + 1000; }
    if (chaineGIR[7] == 'C') { groupe = groupe + 400; }
    if (chaineGIR[2] == 'B') { groupe = groupe + 16; }
    if (chaineGIR[3] == 'B') { groupe = groupe + 16; }
    if (chaineGIR[4] == 'B') { groupe = groupe + 20; }
    if (chaineGIR[5] == 'B') { groupe = groupe + 20; }
    if (chaineGIR[6] == 'B') { groupe = groupe + 200; }
    if (chaineGIR[7] == 'B') { groupe = groupe + 40; }
    // Rang groupe C
    if (groupe >= 1700) { rang = 5; }
    if ((groupe >= 1432) && (groupe <= 1699)) { rang = 6; }
    if (rang != 0)
    { return getRang(rang); }

    // Teste UneChaine pour le groupe D
    groupe = 0;
    if (chaineGIR[4] == 'C') { groupe = groupe + 2000; }
    if (chaineGIR[5] == 'C') { groupe = groupe + 400; }
    if (chaineGIR[6] == 'C') { groupe = groupe + 2000; }
    if (chaineGIR[7] == 'C') { groupe = groupe + 200; }
    if (chaineGIR[4] == 'B') { groupe = groupe + 200; }
    if (chaineGIR[5] == 'B') { groupe = groupe + 200; }
    if (chaineGIR[6] == 'B') { groupe = groupe + 200; }
    // Rang groupe D
    if (groupe >= 2400)
    {  rang = 7;
        return getRang(rang);
    }
    else { rang = 0; }

    // Teste UneChaine pour le groupe E
    groupe = 0;
    if (chaineGIR[0] == 'C') { groupe = 400; }
    if (chaineGIR[1] == 'C') { groupe = groupe + 400; }
    if (chaineGIR[2] == 'C') { groupe = groupe + 400; }
    if (chaineGIR[3] == 'C') { groupe = groupe + 400; }
    if (chaineGIR[4] == 'C') { groupe = groupe + 400; }
    if (chaineGIR[5] == 'C') { groupe = groupe + 800; }
    if (chaineGIR[6] == 'C') { groupe = groupe + 800; }
    if (chaineGIR[7] == 'C') { groupe = groupe + 200; }
    if (chaineGIR[2] == 'B') { groupe = groupe + 100; }
    if (chaineGIR[3] == 'B') { groupe = groupe + 100; }
    if (chaineGIR[4] == 'B') { groupe = groupe + 100; }
    if (chaineGIR[5] == 'B') { groupe = groupe + 100; }
    if (chaineGIR[6] == 'B') { groupe = groupe + 100; }
    // Rang groupe E
    if (groupe >= 1200)
    { rang = 8;
        return getRang(rang);
    }
    else { rang = 0 ; }

    // ' Teste UneChaine pour le groupe F
    groupe = 0;
    if (chaineGIR[0] == 'C') { groupe = 200; }
    if (chaineGIR[1] == 'C') { groupe = groupe + 200; }
    if (chaineGIR[2] == 'C') { groupe = groupe + 500; }
    if (chaineGIR[3] == 'C') { groupe = groupe + 500; }
    if (chaineGIR[4] == 'C') { groupe = groupe + 500; }
    if (chaineGIR[5] == 'C') { groupe = groupe + 500; }
    if (chaineGIR[6] == 'C') { groupe = groupe + 500; }
    if (chaineGIR[7] == 'C') { groupe = groupe + 200; }
    if (chaineGIR[0] == 'B') { groupe = groupe + 100; }
    if (chaineGIR[1] == 'B') { groupe = groupe + 100; }
    if (chaineGIR[2] == 'B') { groupe = groupe + 100; }
    if (chaineGIR[3] == 'B') { groupe = groupe + 100; }
    if (chaineGIR[4] == 'B') { groupe = groupe + 100; }
    if (chaineGIR[5] == 'B') { groupe = groupe + 100; }
    if (chaineGIR[6] == 'B') { groupe = groupe + 100; }
    // Rang groupe F
    if (groupe >= 800)
    {  rang = 9 ;
        return getRang(rang);
    }
    else { rang = 0; }

    // Teste UneChaine pour le groupe G
    groupe = 0;
    if (chaineGIR[0] == 'C') { groupe = 150; }
    if (chaineGIR[1] == 'C') { groupe = groupe + 150; }
    if (chaineGIR[2] == 'C') { groupe = groupe + 300; }
    if (chaineGIR[3] == 'C') { groupe = groupe + 300; }
    if (chaineGIR[4] == 'C') { groupe = groupe + 500; }
    if (chaineGIR[5] == 'C') { groupe = groupe + 500; }
    if (chaineGIR[6] == 'C') { groupe = groupe + 400; }
    if (chaineGIR[7] == 'C') { groupe = groupe + 200; }
    if (chaineGIR[2] == 'B') { groupe = groupe + 200; }
    if (chaineGIR[3] == 'B') { groupe = groupe + 200; }
    if (chaineGIR[4] == 'B') { groupe = groupe + 200; }
    if (chaineGIR[5] == 'B') { groupe = groupe + 200; }
    if (chaineGIR[6] == 'B') { groupe = groupe + 200; }
    if (chaineGIR[7] == 'B') { groupe = groupe + 100; }
    // Rang groupe G
    if (groupe >= 650)
    {  rang = 10;
        return getRang(rang);
    }
    else { rang = 0; }

    // Teste UneChaine pour le groupe H
    groupe = 0;
    if (chaineGIR[2] == 'C') { groupe = groupe + 3000; }
    if (chaineGIR[3] == 'C') { groupe = groupe + 3000; }
    if (chaineGIR[4] == 'C') { groupe = groupe + 3000; }
    if (chaineGIR[5] == 'C') { groupe = groupe + 3000; }
    if (chaineGIR[6] == 'C') { groupe = groupe + 1000; }
    if (chaineGIR[7] == 'C') { groupe = groupe + 1000; }
    if (chaineGIR[2] == 'B') { groupe = groupe + 2000; }
    if (chaineGIR[3] == 'B') { groupe = groupe + 2000; }
    if (chaineGIR[4] == 'B') { groupe = groupe + 2000; }
    if (chaineGIR[5] == 'B') { groupe = groupe + 2000; }
    if (chaineGIR[6] == 'B') { groupe = groupe + 2000; }
    if (chaineGIR[7] == 'B') { groupe = groupe + 1000; }
    // Rang groupe H
    if (groupe >= 4000){ rang = 11; }
    if (groupe >= 2000 && groupe <= 3999) { rang = 12; }
    if (groupe < 2000) { rang = 13; }
    return getRang(rang);

}



GirScore::GirScore() : d(new GirScorePrivate)
{
    d->m_testValidity = true;
    d->m_valid = false;
}

GirScore::~GirScore()
{
}

bool GirScore::isNull() const
{
    return d->m_coherence.isEmpty() &&
            d->m_orientation.isEmpty() &&
            d->m_toilette.isEmpty() &&
            d->m_habillage.isEmpty() &&
            d->m_alimentation.isEmpty() &&
            d->m_elimination.isEmpty() &&
            d->m_transferts.isEmpty() &&
            d->m_interieur.isEmpty();
}

static bool isValidSubScore(const QString &s)
{
//    qWarning() << "test" << s << !s.contains(QRegExp("[^ABCabc]"));
    return !s.contains(QRegExp("[^ABCabc]"));
}

bool GirScore::isValid() const
{
    if (isNull())
        return false;

    if (!d->m_testValidity)
        return d->m_valid;

    d->m_testValidity = false;
    d->m_valid = false;

    // Upper all
//    qWarning() << d->m_coherence <<
//            d->m_orientation <<
//            d->m_toilette <<
//            d->m_habillage <<
//            d->m_alimentation <<
//            d->m_elimination <<
//            d->m_transferts <<
//            d->m_interieur;

    d->m_orientation = d->m_orientation.toUpper();
    d->m_coherence = d->m_coherence.toUpper();
    d->m_toilette = d->m_toilette.toUpper();
    d->m_habillage = d->m_habillage.toUpper();
    d->m_alimentation = d->m_alimentation.toUpper();
    d->m_elimination = d->m_elimination.toUpper();
    d->m_transferts = d->m_transferts.toUpper();
    d->m_interieur = d->m_interieur.toUpper();

//    qWarning() << "testing lengths";
    // Test lenghts
    if ( (d->m_coherence.length() != 1) ||
         (d->m_orientation.length() != 1) ||
         (d->m_toilette.length() != 2) ||
         (d->m_habillage.length() != 3) ||
         (d->m_alimentation.length() != 2) ||
         (d->m_elimination.length() != 2) ||
         (d->m_transferts.length() != 1) ||
         (d->m_interieur.length() != 1))
        return false;

//    qWarning() << "testing contents";
    // Test contents
    if ( !isValidSubScore(d->m_coherence) ||
         !isValidSubScore(d->m_orientation) ||
         !isValidSubScore(d->m_toilette) ||
         !isValidSubScore(d->m_habillage) ||
         !isValidSubScore(d->m_alimentation) ||
         !isValidSubScore(d->m_elimination) ||
         !isValidSubScore(d->m_transferts) ||
         !isValidSubScore(d->m_interieur))
        return false;

//    qWarning() << "valid";
    d->m_valid = true;
    return true;
}

void GirScore::setValues(const QString &coherence,
                                const QString &orientation,
                                const QString &toilette,
                                const QString &habillage,
                                const QString &alimentation,
                                const QString &elimination,
                                const QString &transferts,
                                const QString &interieur)
{
    d->needNewValidityTesting();
    d->m_coherence = coherence;
    d->m_orientation = orientation;
    d->m_toilette = toilette;
    d->m_habillage = habillage;
    d->m_alimentation = alimentation;
    d->m_elimination = elimination;
    d->m_transferts = transferts;
    d->m_interieur = interieur;
}

void GirScore::setCoherence(const QString &val)
{
    d->needNewValidityTesting();
    d->m_coherence = val;
}

void GirScore::setOrientation(const QString &val)
{
    d->needNewValidityTesting();
    d->m_orientation = val;
}

void GirScore::setToilette(const QString &val)
{
    d->needNewValidityTesting();
    d->m_toilette = val;
}

void GirScore::setHabillage(const QString &val)
{
    d->needNewValidityTesting();
    d->m_habillage = val;
}

void GirScore::setAlimentation(const QString &val)
{
    d->needNewValidityTesting();
    d->m_alimentation = val;
}

void GirScore::setElimination(const QString &val)
{
    d->needNewValidityTesting();
    d->m_elimination = val;
}

void GirScore::setTransferts(const QString &val)
{
    d->needNewValidityTesting();
    d->m_transferts = val;
}

void GirScore::setInterieur(const QString &val)
{
    d->needNewValidityTesting();
    d->m_interieur = val;
}

// Descriptives
void GirScore::setExterieur(const QString &val)
{
    Q_UNUSED(val);
    d->needNewValidityTesting();
    d->m_exterieur= val;
}

void GirScore::setCommunication(const QString &val)
{
    Q_UNUSED(val);
    d->needNewValidityTesting();
    d->m_communication = val;
}

QString GirScore::coherence() const
{
    if (!isValid())
        return QString();
    return d->m_coherence;
}

QString GirScore::orientation() const
{
    if (!isValid())
        return QString();
    return d->m_orientation;
}

QString GirScore::toilette() const
{
    if (!isValid())
        return QString();
    return d->m_toilette;
}

QString GirScore::habillage() const
{
    if (!isValid())
        return QString();
    return d->m_habillage;
}

QString GirScore::alimentation() const
{
    if (!isValid())
        return QString();
    return d->m_alimentation;
}

QString GirScore::elimination() const
{
    if (!isValid())
        return QString();
    return d->m_elimination;
}

QString GirScore::transferts() const
{
    if (!isValid())
        return QString();
    return d->m_transferts;
}

QString GirScore::interieur() const
{
    if (!isValid())
        return QString();
    return d->m_interieur;
}

QString GirScore::exterieur() const
{
    if (!isValid())
        return QString();
    return d->m_exterieur;
}

QString GirScore::communication() const
{
    if (!isValid())
        return QString();
    return d->m_communication;
}

QString GirScore::getGirString() const
{
    if (!isValid())
        return QString::null;

    // chaine = coherence + orientation + toilette + habillage + alimentation + elimination + transferts + interieur + exterieur + communication
    QString chaine = "12345678";

    // Cohérence et orientation
    chaine = d->m_coherence;
    chaine += d->m_orientation;

    // Toilette AA=A CC=C autres=B
    if (d->m_toilette == "AA")
        chaine[GIR_toilette] = 'A';
    else if (d->m_toilette == "CC")
        chaine[GIR_toilette] = 'C';
    else chaine[GIR_toilette] = 'B';

    // Habillage (Haut, Moyen, Bas) AAA=A CCC=C Autres=B
    if (d->m_habillage == "AAA")
        chaine[GIR_habillage] = 'A';
    else if (d->m_habillage == "CCC")
        chaine[GIR_habillage] = 'C';
    else chaine[GIR_habillage] = 'B';

    // Alimentation  SM  AA=A CC=C BC=C  CB=C Autres=B
    if (d->m_alimentation == "AA")
        chaine[GIR_alimentation] = 'A';
    else if (d->m_alimentation.contains("C"))
        chaine[GIR_alimentation] = 'C';
    else chaine[GIR_alimentation] = 'B';

    // Continence  AA=A  CC=C CB_BC=C AC=C  CA=C  BB=B
    if (d->m_elimination == "AA")
        chaine[GIR_elimination] = 'A';
    else if (d->m_elimination.contains("C"))
        chaine[GIR_elimination] = 'C';
    else chaine[GIR_elimination] = 'B';

    // Transferts, déplacements et communication
    chaine += d->m_transferts;
    chaine += d->m_interieur;

//    qWarning() << "gir" << chaine;

    // Test validity of chaine
    if (chaine.length() != 8)
        return QString::null;
    if (!isValidSubScore(chaine))
        return QString::null;

    return chaine;
}

int GirScore::resultingGir() const
{
    return getGIR(getGirString());
}

QString GirScore::explanations(int girScore) const
{
    if ((girScore >= 1) && (girScore<=6)) {
        return d->m_GirExplanations[girScore-1];
    }
    return QString();
}
