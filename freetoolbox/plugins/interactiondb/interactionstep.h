/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef INTERACTIONSTEP_H
#define INTERACTIONSTEP_H

#include <coreplugin/ifullreleasestep.h>
#include <QStringList>
#include <QVector>

namespace Utils {
    class PubMedDownloader;
}

namespace IAMDb {

class InteractionStep : public Core::IFullReleaseStep
{
    Q_OBJECT
public:
    InteractionStep(QObject *parent = 0);

    QString id() const {return "InteractionStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::InteractionDatabase;}

    bool createDir();
    bool cleanFiles();
    bool downloadFiles();
    bool process();
    bool computeModelsAndPopulateDatabase();
    QString processMessage() const {return tr("Drug-Drug Interactions database creation");}

    bool postProcessDownload() {m_ActiveDownloadId = -1; downloadNextSource(); return true;}

    QStringList errors() const {return m_Errors;}

public Q_SLOTS:
    void downloadNextSource();

private:
    QStringList m_Errors;
    bool m_UseProgressDialog;
    int m_ActiveDownloadId;
    QVector<int> m_SourceToDownload;
    Utils::PubMedDownloader *m_Downloader;
};

}  //  End namespace IAMDb

#endif // INTERACTIONSTEP_H
