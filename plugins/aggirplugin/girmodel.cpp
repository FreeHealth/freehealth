#include "girmodel.h"

#include <medicalutils/aggir/girscore.h>

#include <QDebug>

using namespace Gir;
using namespace MedicalUtils::AGGIR;

namespace {
    // Groupes discriminatifs
    const char * const TRANSFERTS = "Transferts";
    const char * const DEP_INT = "Déplacements intérieurs";
    const char * const TOILETTE = "Toilette";
    const char * const ELIMINATION = "Élimination";
    const char * const HABILLAGE = "Habillage";
    const char * const ALIMENTATION = "Alimentation";
    const char * const ORIENTATION = "Orientation";
    const char * const COHERENCE = "Cohérence";

    // Groupes illustratifs
    const char * const CUISINE = "Cuisine";
    const char * const TRAITEMENT = "Suivi du traitement";
    const char * const MENAGE = "Ménage";
    const char * const ALERTER = "Alerter";
    const char * const DEP_EXT = "Déplacements extérieurs";
    const char * const TRANSPORTS = "Transports";
    const char * const ACT_TEMPS_LIBRE = "Activités temps libre";
    const char * const ACHATS = "Achats";
    const char * const GESTION = "Gestion";

    // Sous-groupes
    const char * const HAUT = "haut";
    const char * const MOYEN = "moyen";
    const char * const BAS = "bas";
    const char * const URINAIRE = "urinaire";
    const char * const FECALE = "fécale";
    const char * const SERVIR = "se servir";
    const char * const MANGER = "manger";
    const char * const TEMPS = "dans le temps";
    const char * const ESPACE = "dans l'espace";
    const char * const COMPORTEMENT = "Comportement";
    const char * const COMMUNICATION = "Communication";

    // Checks
    const char * const NFP = "Ne fait pas";
    const char * const S = "Spontanément";
    const char * const T = "Totalement";
    const char * const H = "Habituellement";
    const char * const C = "Correctement";
    const char * const NOPB = "Aucun problème";

    // Datas references
    const int ITEM_KEY = Qt::UserRole + 2;
    const int SUBITEM = Qt::UserRole + 3;
    const int RESPONSE = Qt::UserRole + 4;

}

GirModel::GirModel(QObject *parent) :
        QStandardItemModel(parent),
        m_score(0),
        m_GirScore(new MedicalUtils::AGGIR::NewGirScore),
        m_fullRowCount(0)
{
    // Discriminatives
    m_groups.insertMulti(1, Gir::GirItem(::TRANSFERTS,"","", NewGirScore::Transferts));

    m_groups.insertMulti(2, Gir::GirItem(::DEP_INT,"","", NewGirScore::DeplacementsInterieurs));

    m_groups.insertMulti(3, Gir::GirItem(::TOILETTE,::HAUT,"", NewGirScore::Toilette, NewGirScore::Haut));
    m_groups.insertMulti(3, Gir::GirItem(::TOILETTE,::BAS,"", NewGirScore::Toilette, NewGirScore::Bas));

    m_groups.insertMulti(4, Gir::GirItem(::ELIMINATION,::URINAIRE,"", NewGirScore::Elimination, NewGirScore::Urinaire));
    m_groups.insertMulti(4, Gir::GirItem(::ELIMINATION,::FECALE,"", NewGirScore::Elimination, NewGirScore::Fecale));

    m_groups.insertMulti(5, Gir::GirItem(::HABILLAGE,::HAUT,"", NewGirScore::Habillage, NewGirScore::Haut));
    m_groups.insertMulti(5, Gir::GirItem(::HABILLAGE,::BAS,"", NewGirScore::Habillage, NewGirScore::Moyen));
    m_groups.insertMulti(5, Gir::GirItem(::HABILLAGE,::MOYEN,"", NewGirScore::Habillage, NewGirScore::Bas));

    m_groups.insertMulti(6, Gir::GirItem(::CUISINE,"","", NewGirScore::Cuisine));

    m_groups.insertMulti(7, Gir::GirItem(::ALIMENTATION,::SERVIR,"", NewGirScore::Alimentation, NewGirScore::SeServir));
    m_groups.insertMulti(7, Gir::GirItem(::ALIMENTATION,::MANGER,"", NewGirScore::Alimentation, NewGirScore::Manger));

    m_groups.insertMulti(8, Gir::GirItem(::TRAITEMENT,"","", NewGirScore::Traitement));
    m_groups.insertMulti(9, Gir::GirItem(::MENAGE,"","", NewGirScore::Menage));
    m_groups.insertMulti(10, Gir::GirItem(::ALERTER,"","", NewGirScore::Alerter));
    m_groups.insertMulti(11, Gir::GirItem(::DEP_EXT,"","", NewGirScore::DeplacementsExterieurs));
    m_groups.insertMulti(12, Gir::GirItem(::TRANSPORTS,"","", NewGirScore::Transports));
    m_groups.insertMulti(13, Gir::GirItem(::ACT_TEMPS_LIBRE,"","", NewGirScore::TempsLibre));
    m_groups.insertMulti(14, Gir::GirItem(::ACHATS,"","", NewGirScore::Achats));
    m_groups.insertMulti(15, Gir::GirItem(::GESTION,"","", NewGirScore::Gestion));

    m_groups.insertMulti(16, Gir::GirItem(::ORIENTATION,::TEMPS,"", NewGirScore::Orientation, NewGirScore::Temps));
    m_groups.insertMulti(16, Gir::GirItem(::ORIENTATION,::ESPACE,"", NewGirScore::Orientation, NewGirScore::Espace));

    m_groups.insertMulti(17, Gir::GirItem(::COHERENCE,::COMMUNICATION,"", NewGirScore::Coherence, NewGirScore::Communication));
    m_groups.insertMulti(17, Gir::GirItem(::COHERENCE,::COMPORTEMENT,"", NewGirScore::Coherence, NewGirScore::Comportement));

    QList<int> discriminatives;
    discriminatives << 1 << 2 << 3 << 4 << 5 << 7 << 16 << 17;

    // Construct the tree model
    setColumnCount(8);
    foreach(int i, m_groups.uniqueKeys()) {
        QString gr;
        QStandardItem *groupItem = 0;
        QFont bold;
        bold.setBold(true);
        bool isDiscriminative = discriminatives.contains(i);
        foreach(const GirItem &item, m_groups.values(i)) {
            if (gr.isEmpty()) {
                // create group
                gr = item.group;
                groupItem = new QStandardItem(item.group);
                groupItem->setData(item.girScoreItem, ::ITEM_KEY);
                if (isDiscriminative)
                    groupItem->setFont(bold);
                if (!item.subgroup.isEmpty()) {
                    appendRow(groupItem);
                    ++m_fullRowCount;
                }
            }

            // create checks
            QStandardItem *nfp = new QStandardItem;
            nfp->setCheckable(true);
            nfp->setData(NewGirScore::NeFaitPas, ::RESPONSE);
            nfp->setToolTip(::NFP);
            QStandardItem *s = new QStandardItem;
            s->setCheckable(true);
            s->setData(NewGirScore::Spontanement, ::RESPONSE);
            s->setToolTip(::S);
            QStandardItem *t = new QStandardItem;
            t->setCheckable(true);
            t->setData(NewGirScore::Totalement, ::RESPONSE);
            t->setToolTip(::T);
            QStandardItem *c = new QStandardItem;
            c->setCheckable(true);
            c->setData(NewGirScore::Correctement, ::RESPONSE);
            c->setToolTip(::C);
            QStandardItem *h = new QStandardItem;
            h->setCheckable(true);
            h->setData(NewGirScore::Habituellement, ::RESPONSE);
            h->setToolTip(::H);
            QStandardItem *nopb = new QStandardItem;
            nopb->setCheckable(true);
            nopb->setData(NewGirScore::AucunProbleme, ::RESPONSE);
            nopb->setToolTip(::NOPB);

            QStandardItem *result = new QStandardItem;

            // create subitems if needed
            if (item.subgroup.isEmpty()) {
                appendRow(QList<QStandardItem*>()<<groupItem<<nfp<<s<<t<<c<<h<<nopb<<result);
                ++m_fullRowCount;
            } else {
                QStandardItem *itemModel = new QStandardItem(item.subgroup);
                itemModel->setData(item.girScoreSubItem, ::SUBITEM);
                itemModel->setData(item.girScoreItem, ::ITEM_KEY);
                if (isDiscriminative)
                    itemModel->setFont(bold);
                groupItem->appendRow(QList<QStandardItem*>()<<itemModel<<nfp<<s<<t<<c<<h<<nopb<<result);
                ++m_fullRowCount;
            }
        }
    }

    m_score = new QStandardItem("?");
    appendRow(QList<QStandardItem*>()<<new QStandardItem("Score")
              << new QStandardItem()
              << new QStandardItem()
              << new QStandardItem()
              << new QStandardItem()
              << new QStandardItem()
              << new QStandardItem()
              << m_score
              );
    ++m_fullRowCount;
}

GirModel::~GirModel()
{
    delete m_GirScore;
}

bool GirModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if (role==Qt::CheckStateRole) {
        QStandardItemModel::setData(index,value,role);

        // if index is NFP and value is checked -> set all other checked
        if (index.column() == 1 && value.toInt() == Qt::Checked) {
            for(int i = 2; i < 6; ++i) {
                QModelIndex idx = this->index(index.row(), i, index.parent());
                QStandardItemModel::setData(idx, Qt::Checked, role);
            }
            QModelIndex idx = this->index(index.row(), 6, index.parent());
            QStandardItemModel::setData(idx, Qt::Unchecked, role);
        }

        // if index is NoPb and value is checked -> set all other checked
        if (index.column() == 6 && value.toInt() == Qt::Checked) {
            for(int i = 1; i < 6; ++i) {
                QModelIndex idx = this->index(index.row(), i, index.parent());
                QStandardItemModel::setData(idx, Qt::Unchecked, role);
            }
        }

        // Get the response
        int rep = NewGirScore::AucuneReponse;
        for(int i = 1; i < 7; ++i) {
            QModelIndex idx = this->index(index.row(), i, index.parent());
            QStandardItem *item = itemFromIndex(idx);
            if (idx.data(role).toInt() == Qt::Checked) {
                rep += item->data(::RESPONSE).toInt();
            }
        }
        // get the GirItem
        QStandardItem *item = itemFromIndex(this->index(index.row(), 0, index.parent()));
        qWarning();
        qWarning() << "ITEM: item" << item->data(::ITEM_KEY).toInt() << "sub" << item->data(::SUBITEM).toInt() << "rep" << rep;
        if (!item->data(::ITEM_KEY).isValid())
            return false;
        NewGirScore::Reponses reponses = NewGirScore::Reponses(rep);
        if (item->data(::SUBITEM).isValid()) {
            m_GirScore->setValue(NewGirScore::Item(item->data(::ITEM_KEY).toInt()),
                                 NewGirScore::SubItem(item->data(::SUBITEM).toInt()),
                                 reponses);

            QModelIndex itemScore = this->index(index.parent().row(), 7, index.parent().parent());
            setData(itemScore, m_GirScore->getCodeGir(NewGirScore::Item(item->data(::ITEM_KEY).toInt())));

            QModelIndex subitemScore = this->index(index.row(), 7, index.parent());
            setData(subitemScore, m_GirScore->getCodeGir(NewGirScore::Item(item->data(::ITEM_KEY).toInt()),
                                                         NewGirScore::SubItem(item->data(::SUBITEM).toInt())));
        } else {
            m_GirScore->setValue(NewGirScore::Item(item->data(::ITEM_KEY).toInt()),
                                 reponses);

            QModelIndex itemScore = this->index(index.row(), 7, index.parent());
            setData(itemScore, m_GirScore->getCodeGir(NewGirScore::Item(item->data(::ITEM_KEY).toInt())));
        }
        int gir = m_GirScore->resultingGir();
        if (gir > 0) {
            QModelIndex girIndex = this->index(rowCount()-1, 7);
            setData(girIndex, gir);
            Q_EMIT girCalculated(gir);
        }
        return true;
    }
    return QStandardItemModel::setData(index,value,role);
}

QVariant GirModel::data(const QModelIndex &index, int role) const
{
    if (role==Qt::ToolTipRole) {
        if (index.column() >= 2 && index.column() <= 5) {
            if (index.data(Qt::CheckStateRole).toInt()==Qt::Checked) {
                return "Ne fait pas " + QStandardItemModel::data(index, role).toString();
            } else {
                return "Fait " + QStandardItemModel::data(index, role).toString();
            }
        }
    }
    return QStandardItemModel::data(index, role);
}

