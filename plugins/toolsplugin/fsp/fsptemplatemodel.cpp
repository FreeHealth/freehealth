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
 *   Main developers : Eric Maeker <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Tools::Internal::FspTemplateModel
 */

#include "fsptemplatemodel.h"
#include "fspconstants.h"
#include "fsp.h"
#include "fspprinterdialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>

#include <QDir>

#include <QDebug>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

namespace Tools {
namespace Internal {
struct FspXmlFile {
    QString fileName;
    Fsp fsp;
};

class FspTemplateModelPrivate
{
public:
    FspTemplateModelPrivate(FspTemplateModel *parent) :
        q(parent)
    {
    }
    
    ~FspTemplateModelPrivate()
    {
    }
    
    void toItem(const Fsp &fsp)
    {
        QList<int> ids;
        // Données du modèle
        ids << Fsp::Label;
        QFont bold;
        bold.setBold(true);
        QStandardItem *root = new QStandardItem("Libellé");
        root->setFont(bold);
        QStandardItem *rootcol1 = new QStandardItem(fsp.data(Fsp::Label).toString());
        rootcol1->setFont(bold);
        q->invisibleRootItem()->appendRow(QList<QStandardItem*>() << root << rootcol1);

        // Patient data
        //   any value?
        ids.clear();
        ids <<  Fsp::Patient_FullName
             << Fsp::Patient_FullAddress
             << Fsp::Patient_DateOfBirth
             << Fsp::Patient_Personal_NSS
             << Fsp::Patient_Personal_NSSKey
             << Fsp::Patient_Assure_FullName
             << Fsp::Patient_Assure_NSS
             << Fsp::Patient_Assure_NSSKey
             << Fsp::Patient_Assurance_Number;
        bool add = false;
        for(int i=0; i < ids.count(); ++i) {
            if (!fsp.data(ids.at(i)).isNull()) {
                add = true;
                break;
            }
        }
        if (add) {
            QStandardItem *rootPatient = new QStandardItem("Patient");
            QStringList text;
            text << "Nom"
                 << "Adresse"
                 << "Date de naissance"
                 << "NSS"
                 << "NSS Clé"
                 << "Nom de l'assuré"
                 << "NSS de l'assuré"
                 << "NSS Clé de l'assuré"
                 << "Numéro d'assurance";
            for(int i=0; i < ids.count(); ++i) {
                qWarning() << "----" << fsp.data(ids.at(i)) << ids.at(i);
                if (!fsp.data(ids.at(i)).isNull()
                        && !fsp.data(ids.at(i)).toString().isEmpty()) {
                    QStandardItem *col0 = new QStandardItem(text.at(i));
                    QStandardItem *col1 = new QStandardItem(fsp.data(ids.at(i)).toString());
                    rootPatient->appendRow(QList<QStandardItem*>() << col0 << col1);
                }
            }
            root->appendRow(QList<QStandardItem*>() << rootPatient << new QStandardItem(" "));
        }

        // Conditions
        ids.clear();
        ids << Fsp::Condition_Maladie
            << Fsp::Condition_Maladie_ETM
            << Fsp::Condition_Maladie_ETM_Ald
            << Fsp::Condition_Maladie_ETM_Autre
            << Fsp::Condition_Maladie_ETM_L115
            << Fsp::Condition_Maladie_ETM_Prevention
            << Fsp::Condition_Maladie_ETM_AccidentParTiers_Oui
            << Fsp::Condition_Maladie_ETM_AccidentParTiers_Date
            << Fsp::Condition_Maternite
            << Fsp::Condition_Maternite_Date
            << Fsp::Condition_ATMP
            << Fsp::Condition_ATMP_Number
            << Fsp::Condition_ATMP_Date
            << Fsp::Condition_NouveauMedTraitant
            << Fsp::Condition_MedecinEnvoyeur
            << Fsp::Condition_AccesSpecifique
            << Fsp::Condition_Urgence
            << Fsp::Condition_HorsResidence
            << Fsp::Condition_Remplace
            << Fsp::Condition_HorsCoordination
            << Fsp::Condition_AccordPrealableDate;
        add = false;
        for(int i=0; i < ids.count(); ++i) {
            if (!fsp.data(ids.at(i)).isNull()) {
                add = true;
                break;
            }
        }
        if (add) {
            QStandardItem *rootConds = new QStandardItem("Conditions");
            QStringList text;
            text << "Maladie"
                << "Exonération (oui/non)"
                << "ALD"
                << "Exonération autre"
                << "L115"
                << "Prévention"
                << "Exonération accident"
                << "Exonération accident : date"
                << "Maternité"
                << "Date grossesse ou accouchement"
                << "AT/MP"
                << "AT/MP : numéro"
                << "AT/MP : date"
                << "Nouveau médecin traitant"
                << "Envoyeur"
                << "Accès spécifique"
                << "Urgence"
                << "Hors résidence"
                << "Médecin traitant remplacé"
                << "Hors coordination"
                << "Date entente préalable";
            for(int i=0; i < ids.count(); ++i) {
                if (!fsp.data(ids.at(i)).isNull()) {
                    QStandardItem *col0 = new QStandardItem(text.at(i));
                    QStandardItem *col1 = new QStandardItem(fsp.data(ids.at(i)).toString());
                    rootConds->appendRow(QList<QStandardItem*>() << col0 << col1);
                }
            }
            root->appendRow(QList<QStandardItem*>() << rootConds << new QStandardItem(" "));
        }

        // Paiements
        ids.clear();
        ids << Fsp::Unpaid_PartObligatoire
            << Fsp::Unpaid_PartComplementaire;

        add = false;
        for(int i=0; i < ids.count(); ++i) {
            if (!fsp.data(ids.at(i)).isNull()) {
                add = true;
                break;
            }
        }
        if (add) {
            QStandardItem *rootUpaid = new QStandardItem("DAF");
            QStringList text;
            text << "N'a pas réglé la part obligatoire"
                << "N'a pas réglé la part complémentaire";
            for(int i=0; i < ids.count(); ++i) {
                if (!fsp.data(ids.at(i)).isNull()) {
                    QStandardItem *col0 = new QStandardItem(text.at(i));
                    QStandardItem *col1 = new QStandardItem(fsp.data(ids.at(i)).toString());
                    root->appendRow(QList<QStandardItem*>() << col0 << col1);
                }
            }
            root->appendRow(rootUpaid);
        }

        // Add amounts
        QList<QStandardItem*> amounts;
        QStringList text;
        text << "Date"
             << "Code de l'acte"
             << "Activité"
             << "C.V.Cs.CNPSY..."
             << "Autres actes (1)"
             << "Autres actes (2)"
             << "Montant"
             << "Dépassement"
             << "IKMD"
             << "Déplacement : nombre"
             << "Déplacement : montant";
        for(int i = 0; i < 4; ++i) {
            if (fsp.amountLineData(i, Fsp::Amount_Amount).isNull())
                continue;
            QStandardItem *line = new QStandardItem("Honoraires n° " + QString::number(i+1));
            for(int j=Fsp::Amount_Date; j < Fsp::Amount_MaxData; ++j) {
                if (!fsp.amountLineData(i, j).isNull()) {
                    QStandardItem *col0 = new QStandardItem(text.at(j - Fsp::Amount_Date));
                    QStandardItem *col1 = new QStandardItem(fsp.amountLineData(i, j).toString());
                    line->appendRow(QList<QStandardItem*>() << col0 << col1);
                }
            }
            amounts << line;
        }
        if (amounts.count() > 0) {
            foreach(QStandardItem *line, amounts)
                root->appendRow(QList<QStandardItem*>() << line << new QStandardItem(" "));
        }
    }

    void readDatapackFiles()
    {
        _files.clear();
        _fsps.clear();
        QDir dir(FspPrinterDialog::datapackPath());
        if (!dir.exists())
            return;
        QFileInfoList files = Utils::getFiles(dir, "*.xml", Utils::Recursively);
        foreach(const QFileInfo &info, files) {
            QList <Fsp> fsps = Fsp().fromXmlFile(info.absoluteFilePath());
            foreach(const Fsp &fsp, fsps) {
                toItem(fsp);
            }
            _fsps << fsps;
        }
    }

public:
    QStringList _roFiles;
    QList<Fsp> _fsps;
    QList<FspXmlFile> _files;
    
private:
    FspTemplateModel *q;
};
} // namespace Internal
} // end namespace Account


/*! Constructor of the Tools::Internal::FspTemplateModel class */
FspTemplateModel::FspTemplateModel(QObject *parent) :
    QStandardItemModel(parent),
    d(new FspTemplateModelPrivate(this))
{
    setColumnCount(2);
}

/*! Destructor of the Tools::Internal::FspTemplateModel class */
FspTemplateModel::~FspTemplateModel()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool FspTemplateModel::initialize()
{
    // Read datapack files in $DATAPACK/fsp/*.xml
    d->readDatapackFiles();
    return true;
}

Qt::ItemFlags FspTemplateModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

const Fsp &FspTemplateModel::fsp(const QModelIndex &index) const
{
    QModelIndex parent = index;
    while (parent.isValid()) {
        if (parent.parent().isValid())
            parent = parent.parent();
        else
            break;
    }
    return d->_fsps.at(index.row());
}
