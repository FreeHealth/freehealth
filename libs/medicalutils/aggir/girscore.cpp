/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2015 by Eric MAEKER, MD                                      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "girscore.h"

#include <QRegExp>
#include <QStringList>
#include <QVector>

#include <QDebug>

using namespace MedicalUtils::AGGIR;

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  Computations  ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static bool isValidSubScore(const QString &s)
{
//    qWarning() << "test" << s << !s.contains(QRegExp("[^ABCabc]"));
    return !s.contains(QRegExp("[^ABCabc]"));
}

static int getRang(const int rang)
{
    int GIR_calcule;
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
        return -1;

    QString chaineGIR = chaine;

    rang = 0;

    // Teste UneChaine pour le groupe A
    groupe = 0;
    if (chaineGIR[0] == 'C') { groupe += 2000; }
    if (chaineGIR[1] == 'C') { groupe += 1200; }
    if (chaineGIR[2] == 'C') { groupe += 40; }
    if (chaineGIR[3] == 'C') { groupe += 40; }
    if (chaineGIR[4] == 'C') { groupe += 60; }
    if (chaineGIR[5] == 'C') { groupe += 100; }
    if (chaineGIR[6] == 'C') { groupe += 800; }
    if (chaineGIR[7] == 'C') { groupe += 200; }
    if (chaineGIR[2] == 'B') { groupe += 16; }
    if (chaineGIR[3] == 'B') { groupe += 16; }
    if (chaineGIR[4] == 'B') { groupe += 20; }
    if (chaineGIR[5] == 'B') { groupe += 16; }
    if (chaineGIR[6] == 'B') { groupe += 120; }
    if (chaineGIR[7] == 'B') { groupe += 32; }
    // Rang groupe A
    if (groupe >= 4380) {
        rang = 1; 
    } else if ((groupe >= 4140 ) && (groupe <= 4379)) {
        rang = 2 ; 
    } else if ((groupe >= 3390 ) && (groupe <= 4139)) {
        rang = 3 ;
    }

    if (rang != 0) {
        return getRang(rang);
    }

    // Teste UneChaine pour le groupe B
    groupe = 0;
    if (chaineGIR[0] == 'C') { groupe += 1500; }
    if (chaineGIR[1] == 'C') { groupe += 1200; }
    if (chaineGIR[2] == 'C') { groupe += 40; }
    if (chaineGIR[3] == 'C') { groupe += 40; }
    if (chaineGIR[4] == 'C') { groupe += 60; }
    if (chaineGIR[5] == 'C') { groupe += 100; }
    if (chaineGIR[6] == 'C') { groupe += 800;  }
    if (chaineGIR[7] == 'C') { groupe -= 80; }
    if (chaineGIR[0] == 'B') { groupe += 320; }
    if (chaineGIR[1] == 'B') { groupe += 120; }
    if (chaineGIR[2] == 'B') { groupe += 16; }
    if (chaineGIR[3] == 'B') { groupe += 16; }
    if (chaineGIR[4] == 'B') { groupe += 0; }
    if (chaineGIR[5] == 'B') { groupe += 16; }
    if (chaineGIR[6] == 'B') { groupe += 120; }
    if (chaineGIR[7] == 'B') { groupe -= 40; }
    // ' Rang groupe B
    if (groupe >= 2016) {
        rang = 4;
        return getRang(rang);
    } else {
        rang = 0; 
    }

    // Teste UneChaine pour le groupe C
    groupe = 0;
    if (chaineGIR[2] == 'C') { groupe += 40; }
    if (chaineGIR[3] == 'C') { groupe += 40; }
    if (chaineGIR[4] == 'C') { groupe += 60; }
    if (chaineGIR[5] == 'C') { groupe += 160; }
    if (chaineGIR[6] == 'C') { groupe += 1000; }
    if (chaineGIR[7] == 'C') { groupe += 400; }
    if (chaineGIR[2] == 'B') { groupe += 16; }
    if (chaineGIR[3] == 'B') { groupe += 16; }
    if (chaineGIR[4] == 'B') { groupe += 20; }
    if (chaineGIR[5] == 'B') { groupe += 20; }
    if (chaineGIR[6] == 'B') { groupe += 200; }
    if (chaineGIR[7] == 'B') { groupe += 40; }
    // Rang groupe C
    if (groupe >= 1700) {
        rang = 5; 
    } else if ((groupe >= 1432) && (groupe <= 1699)) { 
        rang = 6; 
    }
    if (rang != 0) {
        return getRang(rang); 
    }

    // Teste UneChaine pour le groupe D
    groupe = 0;
    if (chaineGIR[4] == 'C') { groupe += 2000; }
    if (chaineGIR[5] == 'C') { groupe += 400; }
    if (chaineGIR[6] == 'C') { groupe += 2000; }
    if (chaineGIR[7] == 'C') { groupe += 200; }
    if (chaineGIR[4] == 'B') { groupe += 200; }
    if (chaineGIR[5] == 'B') { groupe += 200; }
    if (chaineGIR[6] == 'B') { groupe += 200; }
    // Rang groupe D
    if (groupe >= 2400) {
        rang = 7;
        return getRang(rang);
    }
    rang = 0; 

    // Teste UneChaine pour le groupe E
    groupe = 0;
    if (chaineGIR[0] == 'C') { groupe += 400; }
    if (chaineGIR[1] == 'C') { groupe += 400; }
    if (chaineGIR[2] == 'C') { groupe += 400; }
    if (chaineGIR[3] == 'C') { groupe += 400; }
    if (chaineGIR[4] == 'C') { groupe += 400; }
    if (chaineGIR[5] == 'C') { groupe += 800; }
    if (chaineGIR[6] == 'C') { groupe += 800; }
    if (chaineGIR[7] == 'C') { groupe += 200; }
    if (chaineGIR[2] == 'B') { groupe += 100; }
    if (chaineGIR[3] == 'B') { groupe += 100; }
    if (chaineGIR[4] == 'B') { groupe += 100; }
    if (chaineGIR[5] == 'B') { groupe += 100; }
    if (chaineGIR[6] == 'B') { groupe += 100; }
    // Rang groupe E
    if (groupe >= 1200) {
        rang = 8;
        return getRang(rang);
    }
    rang = 0 ;

    // ' Teste UneChaine pour le groupe F
    groupe = 0;
    if (chaineGIR[0] == 'C') { groupe += 200; }
    if (chaineGIR[1] == 'C') { groupe += 200; }
    if (chaineGIR[2] == 'C') { groupe += 500; }
    if (chaineGIR[3] == 'C') { groupe += 500; }
    if (chaineGIR[4] == 'C') { groupe += 500; }
    if (chaineGIR[5] == 'C') { groupe += 500; }
    if (chaineGIR[6] == 'C') { groupe += 500; }
    if (chaineGIR[7] == 'C') { groupe += 200; }
    if (chaineGIR[0] == 'B') { groupe += 100; }
    if (chaineGIR[1] == 'B') { groupe += 100; }
    if (chaineGIR[2] == 'B') { groupe += 100; }
    if (chaineGIR[3] == 'B') { groupe += 100; }
    if (chaineGIR[4] == 'B') { groupe += 100; }
    if (chaineGIR[5] == 'B') { groupe += 100; }
    if (chaineGIR[6] == 'B') { groupe += 100; }
    // Rang groupe F
    if (groupe >= 800) {
        rang = 9 ;
        return getRang(rang);
    }
    rang = 0;

    // Teste UneChaine pour le groupe G
    groupe = 0;
    if (chaineGIR[0] == 'C') { groupe += 150; }
    if (chaineGIR[1] == 'C') { groupe += 150; }
    if (chaineGIR[2] == 'C') { groupe += 300; }
    if (chaineGIR[3] == 'C') { groupe += 300; }
    if (chaineGIR[4] == 'C') { groupe += 500; }
    if (chaineGIR[5] == 'C') { groupe += 500; }
    if (chaineGIR[6] == 'C') { groupe += 400; }
    if (chaineGIR[7] == 'C') { groupe += 200; }
    if (chaineGIR[2] == 'B') { groupe += 200; }
    if (chaineGIR[3] == 'B') { groupe += 200; }
    if (chaineGIR[4] == 'B') { groupe += 200; }
    if (chaineGIR[5] == 'B') { groupe += 200; }
    if (chaineGIR[6] == 'B') { groupe += 200; }
    if (chaineGIR[7] == 'B') { groupe += 100; }
    // Rang groupe G
    if (groupe >= 650) {
        rang = 10;
        return getRang(rang);
    }
    rang = 0;

    // Teste UneChaine pour le groupe H
    groupe = 0;
    if (chaineGIR[2] == 'C') { groupe += 3000; }
    if (chaineGIR[3] == 'C') { groupe += 3000; }
    if (chaineGIR[4] == 'C') { groupe += 3000; }
    if (chaineGIR[5] == 'C') { groupe += 3000; }
    if (chaineGIR[6] == 'C') { groupe += 1000; }
    if (chaineGIR[7] == 'C') { groupe += 1000; }
    if (chaineGIR[2] == 'B') { groupe += 2000; }
    if (chaineGIR[3] == 'B') { groupe += 2000; }
    if (chaineGIR[4] == 'B') { groupe += 2000; }
    if (chaineGIR[5] == 'B') { groupe += 2000; }
    if (chaineGIR[6] == 'B') { groupe += 2000; }
    if (chaineGIR[7] == 'B') { groupe += 1000; }
    // Rang groupe H
    if (groupe >= 4000) {
        rang = 11;
    } else if (groupe >= 2000 && groupe <= 3999) { 
        rang = 12; 
    } else if (groupe < 2000) { 
        rang = 13; 
    }
    return getRang(rang);
}

//Les groupes 5 et 6 :
//En institution, l'environnement prend normalement en charge la
//continuité de la sécurité des personnes et fournit les denrées
//et autres produits nécessaires à la vie courante.
//A domicile, il en est tout autrement et pour les groupes 5 et 6,
//les variables Déplacements à l’extérieur et Alerter (utilisation
//d'un moyen de communication à distance pour alerter en cas de
//besoins) définissent trois sous-groupes évaluant l'isolement de la
//personne à son domicile.
//C : la personne a besoin d'un tiers pour que soient apportés à
//son logement tous les produits nécessaires à la vie courante ou
//qui ne peut pas alerter correctement son entou- rage en cas d'urgence.
//Il s'agit d'une personne confinée à son domicile.
//B : soit de façon intermittente (dans le temps) soit par rapport à
//la fiabilité de sa propre sécurité ou se son approvisionnement, la
//personne nécessite une surveillance et des actions ponctuelles.
//A : la personne n'a pas de problème majeur et permanent sur ces deux points.


///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  Private Parts  ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
namespace MedicalUtils {
namespace AGGIR {
    namespace {
        enum CreateGirItemGetter {
            CreateGirItem = true,
            DoNotCreateGirItem = false
        };

        // Garder dans l'ordre de construction du girString
        // chaine = coherence + orientation + toilette + habillage + alimentation + elimination + transferts + interieur + exterieur + communication
        const QList<int> discriminatives = QList<int>()
                                           << NewGirScore::Coherence
                                           << NewGirScore::Orientation
                                           << NewGirScore::Toilette
                                           << NewGirScore::Habillage
                                           << NewGirScore::Alimentation
                                           << NewGirScore::Elimination
                                           << NewGirScore::Transferts
                                           << NewGirScore::DeplacementsInterieurs
                                           ;
        const QList<int> illustratives = QList<int>()
                                           << NewGirScore::Cuisine
                                           << NewGirScore::Traitement
                                           << NewGirScore::Menage
                                           << NewGirScore::Alerter
                                           << NewGirScore::DeplacementsExterieurs
                                           << NewGirScore::Transports
                                           << NewGirScore::TempsLibre
                                           << NewGirScore::Achats
                                           << NewGirScore::Gestion;
        const QList<int> twoSubItem = QList<int>()
                                      << NewGirScore::Toilette
                                      << NewGirScore::Elimination
                                      << NewGirScore::Alimentation
                                      << NewGirScore::Orientation
                                      << NewGirScore::Coherence;
        const QList<int> threeSubItem = QList<int>()
                                        << NewGirScore::Habillage;

        static QMultiHash<int, int> subItems;

    }
QMultiHash<int, int> subItems;

struct NewGirItem {
    NewGirItem() :
            item(NewGirScore::NoItemDefined),
            subItem(NewGirScore::NoSubItem),
            reponses(NewGirScore::AucuneReponse)
    {}

    bool isDiscrimative() {return ::discriminatives.contains(item);}
    bool isIllustrative() {return ::illustratives.contains(item);}

    int numberOfSubItems()
    {
        // subItems does not have children
        if (subItem != NewGirScore::NoSubItem) {
            return 0;
        }
        return ::subItems.values(item).count();
    }

    NewGirScore::Item item;
    NewGirScore::SubItem subItem;
    NewGirScore::Reponses reponses;
    QChar computedScore;
};

class NewGirScorePrivate
{
public:
    NewGirScorePrivate() :
        m_testValidity(true),
        m_valid(false)
    {}

    ~NewGirScorePrivate()
    {
        qDeleteAll(m_Items);
        m_Items.clear();
    }

    void needNewValidityTesting()
    {
        m_testValidity=true;
        m_valid=false;
    }

    NewGirItem *girItem(NewGirScore::Item item, CreateGirItemGetter createIfNotExists = CreateGirItem)
    {
        return girItem(item, NewGirScore::NoSubItem, createIfNotExists);
    }

    NewGirItem *girItem(NewGirScore::Item item, NewGirScore::SubItem subItem, CreateGirItemGetter createIfNotExists = CreateGirItem)
    {
        NewGirItem *girItem = 0;
        for(int i=0; i < m_Items.count(); ++i) {
            girItem = m_Items.at(i);
            if (girItem->item == item &&
                girItem->subItem == subItem) {
                return girItem;
            }
        }
        girItem = 0;
        if (createIfNotExists) {
            girItem = new NewGirItem;
            girItem->item = item;
            girItem->subItem = subItem;
            m_Items.append(girItem);
        }
        return girItem;
    }

    // Chaque variable possède trois modalités :
    // - A : fait seul, totalement, habituellement et correctement
    // - B : fait partiellement, ou non habituellement ou non correctement
    // - C : ne fait pas
    QChar calculateItemScore(NewGirItem *item)
    {
        int nbSub = item->numberOfSubItems();
//        qWarning() << "  Calc: item" << item->item << "sub" << item->subItem << "nbSub" << nbSub << "rep" << item->reponses;
        if (!nbSub) {
            if (item->reponses == NewGirScore::AucuneReponse) {
                item->computedScore = '?';
                return '?';
            }
            if (item->reponses & NewGirScore::NeFaitPas) {
                item->computedScore = 'C';
                return 'C';
            }
            if (item->reponses & NewGirScore::AucunProbleme) {
                item->computedScore = 'A';
                return 'A';
            }
            if (item->reponses & NewGirScore::Spontanement &&
                item->reponses & NewGirScore::Totalement &&
                item->reponses & NewGirScore::Habituellement &&
                item->reponses & NewGirScore::Correctement) {
                item->computedScore = 'C';
                return 'C';
            }
            item->computedScore = 'B';
            return 'B';
        } else {
            QString scores;
            for(int i=0;i < m_Items.count(); ++i) {
                NewGirItem *sub = m_Items.at(i);
                if (sub->item == item->item &&
                    sub->subItem != NewGirScore::NoSubItem &&
                    !sub->computedScore.isNull()) {
                    scores.append(sub->computedScore);
                }
            }
            if (!scores.contains(QRegExp("[ABC]"))) {
                return '?';
            }

            if (scores.length() < nbSub)
                return '?';
            switch (item->item) {
            case NewGirScore::Toilette:
                {
                    // Toilette AA=A CC=C autres=B
                    if (scores.compare("AA", Qt::CaseInsensitive)==0) {
                        return 'A';
                    } else if (scores.compare("CC",Qt::CaseInsensitive)==0) {
                        return 'C';
                    }
                    return 'B';
                }
            case NewGirScore::Elimination:
                {
                    // Elimination  AA=A  CC=C CB_BC=C AC=C  CA=C  BB=B
                    if (scores.compare("AA", Qt::CaseInsensitive)==0) {
                        return 'A';
                    } else if (scores.contains("C",Qt::CaseInsensitive)) {
                        return 'C';
                    }
                    return 'B';
                }
            case NewGirScore::Alimentation:
                {
                    // Alimentation  SM  AA=A CC=C BC=C  CB=C Autres=B
                    if (scores.compare("AA", Qt::CaseInsensitive)==0) {
                        return 'A';
                    } else if (scores.compare("CC",Qt::CaseInsensitive)==0) {
                        return 'C';
                    } else if (scores.contains("C",Qt::CaseInsensitive) &&
                               scores.contains("B",Qt::CaseInsensitive) ) {
                        return 'C';
                    }
                    return 'B';
                }
            case NewGirScore::Orientation:
            case NewGirScore::Coherence:
                {
                    // Orientation AA = A ; AB, BA, BB = B ; CC, BC, CB, AC, CA = C
                    // Coherence   AA = A ; AB, BA, BB = B ; CC, BC, CB, AC, CA = C
                    if (scores.compare("AA", Qt::CaseInsensitive)==0) {
                        return 'A';
                    } else if (scores.contains("C",Qt::CaseInsensitive)) {
                        return 'C';
                    }
                    return 'B';
                }
            case NewGirScore::Habillage:
                {
                    // Habillage (Haut, Moyen, Bas) AAA=A CCC=C Autres=B
                    if (scores.compare("AAA", Qt::CaseInsensitive)==0) {
                        return 'A';
                    } else if (scores.compare("CCC",Qt::CaseInsensitive)==0) {
                        return 'C';
                    }
                    return 'B';
                }
            default: break;
            }
        }
        return QChar('?');
    }

    QVector<NewGirItem *> m_Items;
    bool m_testValidity, m_valid;
};



class OldGirScorePrivate
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

QStringList OldGirScorePrivate::m_GirExplanations = QStringList()
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

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  NewGirScore  /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
NewGirScore::NewGirScore() :
        d(new NewGirScorePrivate)
{
    if (::subItems.isEmpty()) {
        ::subItems.insertMulti(NewGirScore::Toilette, NewGirScore::Haut);
        ::subItems.insertMulti(NewGirScore::Toilette, NewGirScore::Bas);
        ::subItems.insertMulti(NewGirScore::Elimination, NewGirScore::Urinaire);
        ::subItems.insertMulti(NewGirScore::Elimination, NewGirScore::Fecale);
        ::subItems.insertMulti(NewGirScore::Habillage, NewGirScore::Haut);
        ::subItems.insertMulti(NewGirScore::Habillage, NewGirScore::Moyen);
        ::subItems.insertMulti(NewGirScore::Habillage, NewGirScore::Bas);
        ::subItems.insertMulti(NewGirScore::Alimentation, NewGirScore::SeServir);
        ::subItems.insertMulti(NewGirScore::Alimentation, NewGirScore::Manger);
        ::subItems.insertMulti(NewGirScore::Orientation, NewGirScore::Temps);
        ::subItems.insertMulti(NewGirScore::Orientation, NewGirScore::Espace);
        ::subItems.insertMulti(NewGirScore::Coherence, NewGirScore::Communication);
        ::subItems.insertMulti(NewGirScore::Coherence, NewGirScore::Comportement);
    }
}

NewGirScore::~NewGirScore()
{
    if (d)
        delete d;
    d = 0;
}

bool NewGirScore::isNull() const
{
    return d->m_Items.isEmpty();
}

bool NewGirScore::isValid() const
{
    return d->m_valid;
}

bool NewGirScore::isComplete() const
{
    NewGirItem *girItem = 0;
    // all discriminatives (with subItems)
    for(int i = 0; i < ::discriminatives.count(); ++i) {
        int itemId = discriminatives.at(i);
        girItem = d->girItem(NewGirScore::Item(itemId), ::DoNotCreateGirItem);
        if (!girItem)
            return false;
        // check subitems
        QList<int> subs = ::subItems.values(itemId);
        if (subs.count() > 0) {
            for(int i = 0; i < subs.count(); ++i) {
                girItem = d->girItem(NewGirScore::Item(itemId), NewGirScore::SubItem(subs.at(i)), ::DoNotCreateGirItem);
                if (!girItem)
                    return false;
            }
        } else {
            girItem = d->girItem(NewGirScore::Item(itemId), ::DoNotCreateGirItem);
            if (!girItem)
                return false;
        }
    }

    // all illustratives
    for(int i = 0; i < ::illustratives.count(); ++i) {
        int itemId = illustratives.at(i);
        girItem = d->girItem(NewGirScore::Item(itemId), ::DoNotCreateGirItem);
        if (!girItem)
            return false;
    }
    return true;
}

void NewGirScore::clear()
{
    qDeleteAll(d->m_Items);
    d->m_Items.clear();
}

static NewGirScore::Reponses correctedReponse(const NewGirScore::Reponses &rep)
{
    if (rep == NewGirScore::NeFaitPas) {
        return NewGirScore::NeFaitPas | NewGirScore::Spontanement | NewGirScore::Totalement | NewGirScore::Correctement | NewGirScore::Habituellement;
    }
    if (rep & NewGirScore::AucunProbleme) {
        return NewGirScore::AucunProbleme;
    }
    if (rep & NewGirScore::NeFaitPas) {
        if (!(rep & NewGirScore::Spontanement) ||
            !(rep & NewGirScore::Totalement)  ||
            !(rep & NewGirScore::Correctement)  ||
            !(rep & NewGirScore::Habituellement)) {
            return rep ^ NewGirScore::NeFaitPas;
        }
    } else {
        if ((rep & NewGirScore::Spontanement) &&
            (rep & NewGirScore::Totalement) &&
            (rep & NewGirScore::Correctement)  &&
            (rep & NewGirScore::Habituellement)) {
            return NewGirScore::NeFaitPas | NewGirScore::Spontanement | NewGirScore::Totalement | NewGirScore::Correctement | NewGirScore::Habituellement;
        }
    }
    return rep;
}

void NewGirScore::setValue(Item item, SubItem subItem, const Reponses &reponses)
{
    NewGirItem *girItem = d->girItem(item, subItem);
//    qWarning() << "setValue: item" << girItem->item << "sub" << girItem->subItem << "rep" << reponses;
    girItem->reponses = correctedReponse(reponses);
    girItem->computedScore = d->calculateItemScore(girItem);
//    qWarning() << "   Gir ->" << item << subItem << reponses << girItem->computedScore;
}

void NewGirScore::setValue(Item item, const Reponses &reponses)
{
    NewGirItem *girItem = d->girItem(item);
//    qWarning() << "setValue: item" << girItem->item << "nosub  rep" << reponses;
    girItem->reponses = correctedReponse(reponses);
    girItem->computedScore = d->calculateItemScore(girItem);
//    qWarning() << "   Gir[nosub] ->" << item << reponses << girItem->computedScore;
}

NewGirScore::Reponses NewGirScore::reponses(Item item, SubItem subItem) const
{
    NewGirItem *girItem = d->girItem(item, subItem);
    return girItem->reponses;
}

QString NewGirScore::getCodeGir(Item item) const
{
    NewGirItem *girItem = d->girItem(item);
    girItem->computedScore = d->calculateItemScore(girItem);
    return girItem->computedScore;
}

QString NewGirScore::getCodeGir(Item item, SubItem subItem) const
{
    for(int i=0; i<d->m_Items.count();++i) {
        NewGirItem *girItem = d->m_Items.at(i);
        if (girItem->item == item &&
            girItem->subItem == subItem) {
            girItem->computedScore = d->calculateItemScore(girItem);
            return girItem->computedScore;
        }
    }
    return "?";
}

int NewGirScore::resultingGir() const
{
    QString chaine;
    for(int i = 0; i < discriminatives.count(); ++i) {
        NewGirItem *girItem = d->girItem(NewGirScore::Item(discriminatives.at(i)));
        girItem->computedScore = d->calculateItemScore(girItem);
        chaine += girItem->computedScore;
    }
    if (!isValidSubScore(chaine))
        return -1;
    int score = getGIR(chaine);
//    qWarning() << "resultingGir chaine:" << chaine << "score" << score;
    return score;
}

QString NewGirScore::explanations(int girScore) const
{
    Q_UNUSED(girScore);
    // TODO: code here
    return QString();
}

QString NewGirScore::serializeScore() const
{
    if (!isComplete())
        return "IncompleteScore";
    // Format is
    //    item,subItem:reponses;...
    //    item:reponses;...
    QStringList score;
    NewGirItem *girItem = 0;
    // all discriminatives (with subItems)
    for(int i = 0; i < ::discriminatives.count(); ++i) {
        int itemId = discriminatives.at(i);
        QList<int> subs = ::subItems.values(itemId);
        if (subs.count() > 0) {
            for(int i = 0; i < subs.count(); ++i) {
                girItem = d->girItem(NewGirScore::Item(itemId), NewGirScore::SubItem(subs.at(i)), DoNotCreateGirItem);
                score << QString("%1,%2:%3").arg(itemId).arg(subs.at(i)).arg(girItem->reponses);
            }
        } else {
            girItem = d->girItem(NewGirScore::Item(itemId), ::DoNotCreateGirItem);
            score << QString("%1:%2").arg(itemId).arg(girItem->reponses);
        }
    }
    // all illustratives
    for(int i = 0; i < ::illustratives.count(); ++i) {
        int itemId = illustratives.at(i);
        girItem = d->girItem(NewGirScore::Item(itemId), DoNotCreateGirItem);
        score << QString("%1:%2").arg(itemId).arg(girItem->reponses);
    }
    score.sort();
    return score.join(";");
}

bool NewGirScore::setSerializedScore(const QString &score) const
{
    QStringList lines = score.split(";");
    NewGirItem *girItem = 0;
    foreach(const QString &l, lines) {
        QStringList vals = l.split(":");
        if (vals.count()!=2)
            return false;
        if (l.contains(",")) {
            // Item + subItem
            QStringList items = vals.at(0).split(",");
            if (items.count()!=2)
                return false;
            girItem = d->girItem(NewGirScore::Item(items.at(0).toInt()),
                                 NewGirScore::SubItem(items.at(1).toInt()));
            girItem->reponses = NewGirScore::Reponses(vals.at(1).toInt());
            girItem->computedScore = d->calculateItemScore(girItem);
        } else {
            // Item only
            girItem = d->girItem(NewGirScore::Item(vals.at(0).toInt()));
            girItem->reponses = NewGirScore::Reponses(vals.at(1).toInt());
            girItem->computedScore = d->calculateItemScore(girItem);
        }
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  OldGirScore  /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
OldGirScore::OldGirScore() :
        d(new OldGirScorePrivate)
{
    d->m_testValidity = true;
    d->m_valid = false;
}

OldGirScore::~OldGirScore()
{
    if (d)
        delete d;
    d = 0;
}

bool OldGirScore::isNull() const
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

bool OldGirScore::isValid() const
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

void OldGirScore::setValues(const QString &coherence,
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

void OldGirScore::setCoherence(const QString &val)
{
    d->needNewValidityTesting();
    d->m_coherence = val;
}

void OldGirScore::setOrientation(const QString &val)
{
    d->needNewValidityTesting();
    d->m_orientation = val;
}

void OldGirScore::setToilette(const QString &val)
{
    d->needNewValidityTesting();
    d->m_toilette = val;
}

void OldGirScore::setHabillage(const QString &val)
{
    d->needNewValidityTesting();
    d->m_habillage = val;
}

void OldGirScore::setAlimentation(const QString &val)
{
    d->needNewValidityTesting();
    d->m_alimentation = val;
}

void OldGirScore::setElimination(const QString &val)
{
    d->needNewValidityTesting();
    d->m_elimination = val;
}

void OldGirScore::setTransferts(const QString &val)
{
    d->needNewValidityTesting();
    d->m_transferts = val;
}

void OldGirScore::setInterieur(const QString &val)
{
    d->needNewValidityTesting();
    d->m_interieur = val;
}

// Descriptives
void OldGirScore::setExterieur(const QString &val)
{
    Q_UNUSED(val);
    d->needNewValidityTesting();
    d->m_exterieur= val;
}

void OldGirScore::setCommunication(const QString &val)
{
    Q_UNUSED(val);
    d->needNewValidityTesting();
    d->m_communication = val;
}

QString OldGirScore::coherence() const
{
    if (!isValid())
        return QString();
    return d->m_coherence;
}

QString OldGirScore::orientation() const
{
    if (!isValid())
        return QString();
    return d->m_orientation;
}

QString OldGirScore::toilette() const
{
    if (!isValid())
        return QString();
    return d->m_toilette;
}

QString OldGirScore::habillage() const
{
    if (!isValid())
        return QString();
    return d->m_habillage;
}

QString OldGirScore::alimentation() const
{
    if (!isValid())
        return QString();
    return d->m_alimentation;
}

QString OldGirScore::elimination() const
{
    if (!isValid())
        return QString();
    return d->m_elimination;
}

QString OldGirScore::transferts() const
{
    if (!isValid())
        return QString();
    return d->m_transferts;
}

QString OldGirScore::interieur() const
{
    if (!isValid())
        return QString();
    return d->m_interieur;
}

QString OldGirScore::exterieur() const
{
    if (!isValid())
        return QString();
    return d->m_exterieur;
}

QString OldGirScore::communication() const
{
    if (!isValid())
        return QString();
    return d->m_communication;
}

QString OldGirScore::getGirString() const
{
    if (!isValid())
        return QString::null;

    // chaine = coherence + orientation + toilette + habillage + alimentation + elimination + transferts + interieur + exterieur + communication
    QString chaine = "12345678";

    // Cohérence et orientation
    if (!d->m_coherence.isEmpty())
        chaine[GIR_coherence] = d->m_coherence.at(0);
    if (!d->m_orientation.isEmpty())
        chaine[GIR_orientation] = d->m_orientation.at(0);

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

int OldGirScore::resultingGir() const
{
    return getGIR(getGirString());
}

QString OldGirScore::explanations(int girScore) const
{
    if ((girScore >= 1) && (girScore<=6)) {
        return d->m_GirExplanations[girScore-1];
    }
    return QString();
}
