#include "girmodel.h"

#include <medicalutils/aggir/girscore.h>

#include <QDebug>

using namespace Gir;
using namespace MedicalUtils::AGGIR;

namespace {

    enum Columns {
        Column_Label = 0,
        Column_NFP,
        Column_S,
        Column_T,
        Column_C,
        Column_H,
        Column_PasProbleme,
        Column_Result,
        Column_Count
    };

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
    const char * const S = "Spontanément: si la case est coché, signifie que le patient ne fait pas spontanément";
    const char * const T = "Totalement: si la case est coché, signifie que le patient ne fait pas totalement";
    const char * const H = "Habituellement: si la case est coché, signifie que le patient ne fait pas habituellement";
    const char * const C = "Correctement: si la case est coché, signifie que le patient ne fait pas correctement";
    const char * const NOPB = "Aucun problème";

    // Data references
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

    m_groups.insertMulti(3, Gir::GirItem(::TOILETTE,::BAS,"", NewGirScore::Toilette, NewGirScore::Bas));
    m_groups.insertMulti(3, Gir::GirItem(::TOILETTE,::HAUT,"", NewGirScore::Toilette, NewGirScore::Haut));

    m_groups.insertMulti(4, Gir::GirItem(::ELIMINATION,::FECALE,"", NewGirScore::Elimination, NewGirScore::Fecale));
    m_groups.insertMulti(4, Gir::GirItem(::ELIMINATION,::URINAIRE,"", NewGirScore::Elimination, NewGirScore::Urinaire));

    m_groups.insertMulti(5, Gir::GirItem(::HABILLAGE,::BAS,"", NewGirScore::Habillage, NewGirScore::Bas));
    m_groups.insertMulti(5, Gir::GirItem(::HABILLAGE,::MOYEN,"", NewGirScore::Habillage, NewGirScore::Moyen));
    m_groups.insertMulti(5, Gir::GirItem(::HABILLAGE,::HAUT,"", NewGirScore::Habillage, NewGirScore::Haut));

    m_groups.insertMulti(6, Gir::GirItem(::CUISINE,"","", NewGirScore::Cuisine));

    m_groups.insertMulti(7, Gir::GirItem(::ALIMENTATION,::MANGER,"", NewGirScore::Alimentation, NewGirScore::Manger));
    m_groups.insertMulti(7, Gir::GirItem(::ALIMENTATION,::SERVIR,"", NewGirScore::Alimentation, NewGirScore::SeServir));

    m_groups.insertMulti(8, Gir::GirItem(::TRAITEMENT,"","", NewGirScore::Traitement));
    m_groups.insertMulti(9, Gir::GirItem(::MENAGE,"","", NewGirScore::Menage));
    m_groups.insertMulti(10, Gir::GirItem(::ALERTER,"","", NewGirScore::Alerter));
    m_groups.insertMulti(11, Gir::GirItem(::DEP_EXT,"","", NewGirScore::DeplacementsExterieurs));
    m_groups.insertMulti(12, Gir::GirItem(::TRANSPORTS,"","", NewGirScore::Transports));
    m_groups.insertMulti(13, Gir::GirItem(::ACT_TEMPS_LIBRE,"","", NewGirScore::TempsLibre));
    m_groups.insertMulti(14, Gir::GirItem(::ACHATS,"","", NewGirScore::Achats));
    m_groups.insertMulti(15, Gir::GirItem(::GESTION,"","", NewGirScore::Gestion));

    m_groups.insertMulti(16, Gir::GirItem(::ORIENTATION,::ESPACE,"", NewGirScore::Orientation, NewGirScore::Espace));
    m_groups.insertMulti(16, Gir::GirItem(::ORIENTATION,::TEMPS,"", NewGirScore::Orientation, NewGirScore::Temps));

    m_groups.insertMulti(17, Gir::GirItem(::COHERENCE,::COMPORTEMENT,"", NewGirScore::Coherence, NewGirScore::Comportement));
    m_groups.insertMulti(17, Gir::GirItem(::COHERENCE,::COMMUNICATION,"", NewGirScore::Coherence, NewGirScore::Communication));

    QList<int> discriminatives;
    discriminatives << 1 << 2 << 3 << 4 << 5 << 7 << 16 << 17;

    QFont bold;
    bold.setBold(true);

    setColumnCount(Column_Count);
    setHeaderData(Column_Label, Qt::Horizontal, "Items et sous-items");
    setHeaderData(Column_NFP, Qt::Horizontal, "Ne fait pas");
    setHeaderData(Column_NFP, Qt::Horizontal, "Ne réalise pas l'activité", Qt::ToolTipRole);

    setHeaderData(Column_S, Qt::Horizontal, "S");
    setHeaderData(Column_T, Qt::Horizontal, "T");
    setHeaderData(Column_C, Qt::Horizontal, "C");
    setHeaderData(Column_H, Qt::Horizontal, "H");

    setHeaderData(Column_S, Qt::Horizontal, ::S, Qt::ToolTipRole);
    setHeaderData(Column_T, Qt::Horizontal, ::T, Qt::ToolTipRole);
    setHeaderData(Column_C, Qt::Horizontal, ::C, Qt::ToolTipRole);
    setHeaderData(Column_H, Qt::Horizontal, ::H, Qt::ToolTipRole);

    setHeaderData(Column_PasProbleme, Qt::Horizontal, "Aucun problème");
    setHeaderData(Column_PasProbleme, Qt::Horizontal, "Aucun problème", Qt::ToolTipRole);
    setHeaderData(Column_Result, Qt::Horizontal, "Score");

    for(int i = 0; i < Column_Count; ++i) {
        setHeaderData(i, Qt::Horizontal, bold, Qt::FontRole);
    }

    // Construct the tree model
    foreach(int i, m_groups.uniqueKeys()) {
        QString gr;
        QStandardItem *groupItem = 0;
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
            if (isDiscriminative) {
                result->setFont(bold);
            }

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

void GirModel::clearGirScore()
{
    beginResetModel();
    m_GirScore->clear();
    endResetModel();
}

void GirModel::setStringfiedGirScore(const QString &score)
{
    beginResetModel();
    if (m_GirScore) {
        m_GirScore->setSerializedScore(score);
        int gir = m_GirScore->resultingGir();
        if (gir > 0)
            Q_EMIT girCalculated(gir);
    }
    endResetModel();
}

QString GirModel::stringfiedGirScore() const
{
    if (m_GirScore)
        return m_GirScore->serializeScore();
    return QString();
}

bool GirModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role==Qt::CheckStateRole) {
        if (index.column() >= Column_NFP && index.column() <= Column_PasProbleme) {
            // get the GirItem
//            QStandardItem *item = itemFromIndex(index);
            QStandardItem *forGirItem = itemFromIndex(this->index(index.row(), Column_Label, index.parent()));
            if (!forGirItem->data(::ITEM_KEY).isValid())
                return false;

            // calculate actual response
            int rep = NewGirScore::AucuneReponse;
            switch (index.column()) {
            case Column_PasProbleme:
                {
                    rep = NewGirScore::AucunProbleme;
                    break;
                }
            case Column_NFP:
                {
                    if (value.toInt()==Qt::Checked) {
                        rep = NewGirScore::NeFaitPas;
                    } else {
                        for(int i = Column_NFP; i < Column_PasProbleme; ++i) {
                            QModelIndex idx = this->index(index.row(), i, index.parent());
                            QStandardItem *itemResponse = itemFromIndex(idx);
                            if (idx.column()==index.column()) {
                                if (value.toInt()==Qt::Checked)
                                    rep += itemResponse->data(::RESPONSE).toInt();
                            } else if (idx.data(role).toInt() == Qt::Checked) {
                                rep += itemResponse->data(::RESPONSE).toInt();
                            }
                        }
                    }
                    break;
                }
            default:
                {
                    for(int i = Column_NFP; i < Column_PasProbleme; ++i) {
                        QModelIndex idx = this->index(index.row(), i, index.parent());
                        QStandardItem *itemResponse = itemFromIndex(idx);
                        if (idx.column()==index.column()) {
                            if (value.toInt()==Qt::Checked)
                                rep += itemResponse->data(::RESPONSE).toInt();
                        } else if (idx.data(role).toInt() == Qt::Checked) {
                            rep += itemResponse->data(::RESPONSE).toInt();
                        }
                    }
                    break;
                }
            }
            NewGirScore::Reponses reponses = NewGirScore::Reponses(rep);
            if (forGirItem->data(::SUBITEM).isValid()) {
                m_GirScore->setValue(NewGirScore::Item(forGirItem->data(::ITEM_KEY).toInt()),
                                     NewGirScore::SubItem(forGirItem->data(::SUBITEM).toInt()),
                                     reponses);
                QModelIndex master = this->index(index.parent().row(), 7, index.parent().parent());
                Q_EMIT dataChanged(master, master);
            } else {
                m_GirScore->setValue(NewGirScore::Item(forGirItem->data(::ITEM_KEY).toInt()),
                                     reponses);
            }
            QModelIndex topLeft = this->index(index.row(), 1, index.parent());
            QModelIndex bottomRight = this->index(index.row(), 7, index.parent());
            Q_EMIT dataChanged(topLeft, bottomRight);
            if (m_GirScore->isComplete()) {
                Q_EMIT girCalculated(m_GirScore->resultingGir());
            }
            return true;
        }
        return true;
    }
    return QStandardItemModel::setData(index,value,role);
}

QVariant GirModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role==Qt::DisplayRole) {
        if (index.column() == Column_Result) {
            QStandardItem *item = itemFromIndex(index);
            if (item == m_score) {
                if (m_GirScore->isComplete())
                    return m_GirScore->resultingGir();
            }
            QStandardItem *forGirItem = itemFromIndex(this->index(index.row(), Column_Label, index.parent()));
            if (forGirItem->data(::SUBITEM).isNull())
                return m_GirScore->getCodeGir(NewGirScore::Item(forGirItem->data(::ITEM_KEY).toInt()));
            else
                return m_GirScore->getCodeGir(NewGirScore::Item(forGirItem->data(::ITEM_KEY).toInt()),
                                              NewGirScore::SubItem(forGirItem->data(::SUBITEM).toInt()));
        }
        return QStandardItemModel::data(index, role);
    }
    if (role==Qt::CheckStateRole) {
        if (!m_GirScore)
            return QVariant();
        if (index.column() >= Column_NFP && index.column() <= Column_PasProbleme) {
            QStandardItem *item = itemFromIndex(index);
            if (!item->isCheckable())
                return QVariant();
            QStandardItem *forGirItem = itemFromIndex(this->index(index.row(), Column_Label, index.parent()));
            // Get the response
            NewGirScore::Reponses rep = m_GirScore->reponses(NewGirScore::Item(forGirItem->data(::ITEM_KEY).toInt()), NewGirScore::SubItem(forGirItem->data(::SUBITEM).toInt()));
            if (rep & NewGirScore::Reponses(item->data(::RESPONSE).toInt())) {
                return Qt::Checked;
            }
            return Qt::Unchecked;
        }
        return QVariant();
    }
    if (role==Qt::ToolTipRole) {
        if (index.column() >= Column_S && index.column() <= Column_H) {
            if (index.data(Qt::CheckStateRole).toInt()==Qt::Checked) {
                return QString("Ne fait pas " + QStandardItemModel::data(index, role).toString());
            } else {
                return QString("Fait " + QStandardItemModel::data(index, role).toString());
            }
        }
    }
    return QStandardItemModel::data(index, role);
}

Qt::ItemFlags GirModel::flags(const QModelIndex &index) const
{
    QStandardItem *item = itemFromIndex(index);
    if (item->isCheckable())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QString GirModel::rowToHtml(int i, const QModelIndex &parent, bool discriminative) const
{
    Q_UNUSED(discriminative);
    QString nfp, s, t, c, h, ok, score, indent;
    if (data(index(i, 1, parent), Qt::CheckStateRole).toInt()==Qt::Checked) {
        nfp = "■";
    } else {
        nfp = "⎕";
    }
    if (data(index(i, 2, parent), Qt::CheckStateRole).toInt()==Qt::Checked) {
        s = "■";
    } else {
        s = "⎕";
    }

    if (data(index(i, 3, parent), Qt::CheckStateRole).toInt()==Qt::Checked) {
        t = "■";
    } else {
        t = "⎕";
    }

    if (data(index(i, 4, parent), Qt::CheckStateRole).toInt()==Qt::Checked) {
        c = "■";
    } else {
        c = "⎕";
    }

    if (data(index(i, 5, parent), Qt::CheckStateRole).toInt()==Qt::Checked) {
        h = "■";
    } else {
        h = "⎕";
    }

    if (data(index(i, 6, parent), Qt::CheckStateRole).toInt()==Qt::Checked) {
        ok = "■";
    } else {
        ok = "⎕";
    }

    score = data(index(i, 7, parent)).toString();
    if (parent.isValid())
        indent = "&nbsp;&nbsp;&nbsp;&nbsp;";
    QString style;
    if (discriminative)
        style += "font-weight: 600;";

    return QString("<td><span style=\"%10\">%9%1</span></td>"  // Label
                    "<td><center>%2</center></td>"  // NFP
                    "<td><center>%3</center></td>"  // S
                    "<td><center>%4</center></td>"  // T
                    "<td><center>%5</center></td>"  // C
                    "<td><center>%6</center></td>"  // H
                    "<td><center>%7</center></td>"  // Ok
                    "<td><center>%8</center></td>"  // Score
                    )
             .arg(data(index(i, 0, parent)).toString())
             .arg(nfp)
             .arg(s)
             .arg(t)
             .arg(c)
             .arg(h)
             .arg(ok)
             .arg(score.remove("?"))
             .arg(indent)
            .arg(style)
             ;
}

QString GirModel::toHtml(const QString &mask) const
{
    QString content;
    if (!mask.isEmpty()) {
        // Use the mask
        bool html = mask.contains("</body>");
        content = mask;
        QStringList adverbs;
        adverbs << "NFP" << "S" << "T" << "C" << "H" << "F";

        QString item;
        QString token;
        QString data;
        for(int i = 0; i < rowCount() - 1; ++i) {
            QModelIndex index = this->index(i,0);
            // score
            item = this->data(index).toString();
            token = "[[" + item + "]]";
            data = this->data(this->index(i, 7)).toString().remove("?");
            content = content.replace(token, data, Qt::CaseInsensitive);
            if (!hasChildren(index)) {
                for(int j=0; j < adverbs.count(); ++j) {
                    token = "[[" + item + "." + adverbs.at(j) + "]]";
                    data.clear();
                    if (this->data(this->index(i, j+1), Qt::CheckStateRole) == Qt::Checked) {
                        if (html)
                            data = "<b>■</b>"; // ■ █
                        else
                            data = "█"; // ■ █
                    } else {
                        data = "⎕";
                    }
                    content = content.replace(token, data, Qt::CaseInsensitive);
                }
                data.clear();
            }

            // add children
            for(int z=0; z < rowCount(this->index(i,0)); ++z) {
                QModelIndex subindex = this->index(z, 0, this->index(i,0));
                QString sub = this->data(subindex).toString();
                token = "[[" + item + "." + sub + "]]";
                data = this->data(this->index(subindex.row(), 7, subindex.parent())).toString().remove("?");
                content = content.replace(token, data, Qt::CaseInsensitive);

                for(int j=0; j < adverbs.count(); ++j) {
                    token = "[[" + item + "." + sub + "." + adverbs.at(j) + "]]";
                    data.clear();
                    if (this->data(this->index(subindex.row(), j+1, subindex.parent()), Qt::CheckStateRole) == Qt::Checked) {
                        if (html)
                            data = "<b>■</b>"; // ■ █
                        else
                            data = "█"; // ■ █
                    } else {
                        data = "⎕";
                    }
                    content = content.replace(token, data, Qt::CaseInsensitive);
                }
            }

            data.clear();
            token = "[[AGGIR.SCORE]]";
            if (m_GirScore->isComplete())
                data = QString::number(m_GirScore->resultingGir());
            else
                data = "Score Incomplet";
            content = content.replace(token, data, Qt::CaseInsensitive);
        }

    } else {
        // No mask, create a default HTML
        QList<int> discriminatives;
        discriminatives << 0 << 1 << 2 << 3 << 4 << 6 << 15 << 16;
        // ⍌⎕■☒☑
        QStringList lines;
        for(int i = 0; i < rowCount() - 1; ++i) {
            if (hasChildren(index(i,0))) {
                // add title
                if (discriminatives.contains(i)) {
                    lines << QString("<td colspan=7><b>%1</b></td>"
                                     "<td><center>%2</center></td>")
                             .arg(data(index(i, 0)).toString())
                             .arg(data(index(i, 7)).toString().remove("?"))
                             ;
                } else {
                    lines << QString("<td colspan=7>%1</td>"
                                     "<td><center>%2</center></td>")
                             .arg(data(index(i, 0)).toString())
                             .arg(data(index(i, 7)).toString().remove("?"))
                             ;
                }
                // add children
                for(int j=0; j < rowCount(index(i,0)); ++j) {
                    lines << rowToHtml(j, index(i,0), discriminatives.contains(i));
                }
                continue;
            }

            lines << rowToHtml(i, QModelIndex(), discriminatives.contains(i));
        }

        // add score
        lines << QString("<td colspan=7><b>%1</b></td>"
                         "<td><b>%2</b></td>")
                 .arg(tr("Score"))
                 .arg(data(index(rowCount() - 1, Column_Result)).toString())
                 ;

        QString style;
        style = "<style>"
                "#gir {"
                " font-family: Arial, Helvetica, sans-serif;"
                " /*witdh:100%;*/"
                " border-collapse:collapse;"
                "}"
                ""
                "#gir td, #gir th {"
                "  font-size:1em;"
                "  border:1px solid #98bf21;"
                "  padding:2px 5px 2px 5px;"
                "}"
                ""
                "#gir th {"
                "  font-size:1.1em;"
                "  text-align:left;"
                "  padding-top:5px;"
                "  padding-bottom:4px;"
                "  background-color:#A7C942;"
                "  color:#ffffff;"
                "}"
                ""
                "#gir tr.alt td {"
                "color:#000000;background-color:#EAF2D3;}"
                ""
                "#gir td.td1 {width: 20em; }"
                "#gir td.td2 {width: 3em; }"
                "#gir td.td3 {width: 1em; }"
                "#gir td.td4 {width: 1em; }"
                "#gir td.td5 {width: 1em; }"
                "#gir td.td6 {width: 1em; }"
                "#gir td.td7 {width: 1em; }"
                "#gir td.td8 {width: 1em; }"
                ""
                "#gir td.discriminative {font-weight:600}"
                ""
                "</style>"
                ;
        content = style + QString("\n\n"
                                          "<p><div style=\"font-weight:600; text-align:center\">%1</div>\n"
                                          "<table id=\"customers\">\n"
                                          "<thead>\n"
                                          "<tr>\n"
                                          " <td>%2</span></td>\n"  // Label
                                          " <td><center>NFP</center></td>\n"  // NFP
                                          " <td><center>S</center></td>\n"  // S
                                          " <td><center>T</center></td>\n"  // T
                                          " <td><center>C</center></td>\n"  // C
                                          " <td><center>H</center></td>\n"  // H
                                          " <td><center>Fait</center></td>\n"  // Ok
                                          " <td><center>Score</center></td>\n"  // Score
                                          "</tr>\n"
                                          "</thead>\n"
                                          "<tbody>\n"
                                          "<tr>\n\n"
                                          "%3\n\n"
                                          "</tr>\n"
                                          "</tbody>\n"
                                          "</table>\n\n")
                .arg("AGGIR")
                .arg(tr("Label"))
                .arg(lines.join("</tr><tr>"));
    }

    return content;
}
