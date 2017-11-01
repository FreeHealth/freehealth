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
#include "bibliocore.h"
#include "bibliobase.h"

#include <utils/pubmeddownloader.h>

using namespace Biblio;
using namespace Internal;

BiblioCore *BiblioCore::m_Instance = 0;

BiblioCore::BiblioCore(QObject *parent) :
    QObject(parent), m_Base(0), m_dld(0)
{
    m_Instance = this;
    m_Base = new BiblioBase(this);
    m_Base->init();
}

void BiblioCore::downloadPubMedData(const QStringList &pmids)
{
    m_ToDownload.clear();
    foreach(const QString &pmid, pmids) {
        if (!m_Base->hasPmid(pmid))
            m_ToDownload << pmid;
    }
    m_ToDownload.removeDuplicates();
    m_ToDownload.removeAll("");
    if (m_dld) {
        delete m_dld;
    } else {
        m_dld = new Utils::PubMedDownloader(this);
        m_dld->setDownloadXmlOnly();
        connect(m_dld, SIGNAL(downloadFinished()), this, SLOT(downloadNext()));
    }
    downloadNext();
}

void BiblioCore::downloadNext()
{
    if (!m_dld->xmlEncoded().isEmpty()) {
        // save to database
        m_Base->save(m_dld->pmid(), m_dld->xmlEncoded());
    }
    if (m_ToDownload.isEmpty()) {
        return;
    }
    m_dld->setPMID(m_ToDownload.takeFirst());
    m_dld->startDownload();
}

QString BiblioCore::xml(const QString &pmid)
{
    return m_Base->getXmlForPMID(pmid);
}
