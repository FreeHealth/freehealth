/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef INTERACTIONSTEP_H
#define INTERACTIONSTEP_H

#include <coreplugin/ifullreleasestep.h>
#include <QStringList>
#include <QVector>
#include <QMultiMap>
class QSqlDatabase;

namespace Utils {
    class PubMedDownloader;
}

namespace IAMDb {
class DrugInteractor;
class DrugDrugInteraction;

class InteractionStep : public Core::IFullReleaseStep
{
    Q_OBJECT
public:
    InteractionStep(QObject *parent = 0);

    QString id() const {return "InteractionStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::InteractionDatabase;}

    bool createDir();
    bool cleanFiles();
    bool downloadFiles(QProgressBar *bar = 0);
    bool process();
    bool computeModelsAndPopulateDatabase();
    QString processMessage() const {return tr("Drug-Drug Interactions database creation");}

    bool postProcessDownload() {return true;} //{m_ActiveDownloadId = -1; downloadNextSource(); return true;}

    QStringList errors() const {return m_Errors;}

public Q_SLOTS:
    void downloadNextSource();

private:
    bool saveAtcClassification(const QList<DrugInteractor *> &interactor);
    bool saveClassDrugInteractor(DrugInteractor *interactor, const QList<DrugInteractor *> &completeList, QSqlDatabase &db, DrugInteractor *parent = 0);
    bool saveDrugDrugInteractions(const QList<DrugInteractor *> &interactor, const QList<DrugDrugInteraction *> ddis);
    bool saveBibliographicReferences();

private:
    QStringList m_Errors;
    bool m_UseProgressDialog;
    QMultiMap<int, QString> m_iamTreePmids; //K=IAM_ID  ;  V=PMIDs
    QMultiMap<int, QString> m_ddiPmids;     //K=IAK_ID  ;  V=PMIDs
    Utils::PubMedDownloader *m_Downloader;
};

}  //  End namespace IAMDb

#endif // INTERACTIONSTEP_H
