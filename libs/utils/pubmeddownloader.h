/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#ifndef PUBMEDDOWNLOADER_H
#define PUBMEDDOWNLOADER_H

#include <utils/global_exporter.h>
#include <QObject>
QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

namespace Utils {

class UTILS_EXPORT PubMedDownloader : public QObject
{
    Q_OBJECT
public:
    explicit PubMedDownloader(QObject *parent = 0);
    ~PubMedDownloader();

    void setPMID(const int pmid) {m_Pmid = QString::number(pmid);}
    void setPMID(const QString &pmid) {m_Pmid = pmid;}
    bool setFullLink(const QString &link);

    QString reference() const {return m_Reference;}
    QString abstract() const {return m_Abstract;}

Q_SIGNALS:
    void downloadFinished();

public Q_SLOTS:
    void startDownload();

private Q_SLOTS:
    void referencesFinished(QNetworkReply *reply);
    void abstractFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
    QString m_Pmid, m_Abstract, m_Reference;
    bool m_DownloadingReferences;
};

}  // End namespace Utils

#endif // PUBMEDDOWNLOADER_H
