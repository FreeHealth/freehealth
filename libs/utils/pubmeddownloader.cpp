/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pubmeddownloader.h"

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkProxy>
#include <QRegExp>
#include <QDomDocument>

using namespace Utils;
using namespace Trans::ConstantTranslations;
//
//  Get textual summary of publication (pubmed)
//  http://www.ncbi.nlm.nih.gov/pubmed/8148870?dopt=docsum&format=text
//

static const char *REFERENCE_URL = "http://www.ncbi.nlm.nih.gov/pubmed/%1?dopt=docsum&format=text";
static const char *ABSTRACT_URL  = "http://www.ncbi.nlm.nih.gov/pubmed/%1?dopt=Abstract&format=text";
static const char *XML_URL       = "http://www.ncbi.nlm.nih.gov/pubmed/%1?dopt=xml&format=text";


PubMedDownloader::PubMedDownloader(QObject *parent) :
    QObject(parent), manager(0), m_DownloadingReferences(false), m_XmlOnly(false)
{
    manager = new QNetworkAccessManager(this);
//    QNetworkProxy prox(QNetworkProxy::HttpProxy, "chaisa1", 8080, "urg", "debut1");
//    manager->setProxy(prox);
}

PubMedDownloader::~PubMedDownloader()
{
}

bool PubMedDownloader::setFullLink(const QString &link)
{
    m_Reference.clear();
    m_Abstract.clear();
    m_Pmid.clear();
    if (!link.startsWith("http://www.ncbi.nlm.nih.gov/pubmed/")) {
//        LOG_ERROR(tr("Wrong PubMed link %1").arg(link));
        return false;
    }
    m_Pmid = link;
    m_Pmid = m_Pmid.remove("http://www.ncbi.nlm.nih.gov/pubmed/");
    if (m_Pmid.contains("?")) {
        m_Pmid = m_Pmid.mid(m_Pmid.indexOf("?"));
    }
    if (m_Pmid.contains(QRegExp("\\D"))) {
//        LOG_ERROR(tr("Wrong PubMed link %1. Extract PMID %2").arg(link).arg(m_Pmid));
        m_Pmid.clear();
        return false;
    }
    return true;
}

void PubMedDownloader::startDownload()
{
    if (m_Pmid.isEmpty()) {
        Q_EMIT downloadFinished();
        return;
    }
    qWarning() << "PubMedDownloader starts downloading of: " + QString(REFERENCE_URL).arg(m_Pmid);
    m_Reference.clear();
    m_Abstract.clear();
    manager->disconnect();
    if (m_XmlOnly) {
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(xmlFinished(QNetworkReply*)));
        manager->get(QNetworkRequest(QUrl(QString(XML_URL).arg(m_Pmid))));
    } else {
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(referencesFinished(QNetworkReply*)));
        manager->get(QNetworkRequest(QUrl(QString(REFERENCE_URL).arg(m_Pmid))));
    }
}

void PubMedDownloader::referencesFinished(QNetworkReply *reply)
{
    qWarning() << "PubMedDownloader Reference" << reply->url();
    m_Reference = reply->readAll();
    int b = m_Reference.indexOf("<pre>\n1: ") + 9;
    int e = m_Reference.indexOf("</pre>", b);
    m_Reference = m_Reference.mid(b, e-b);
    m_Reference.replace("&lt;", "<");
    m_Reference.replace("&gt;", ">");
    manager->disconnect();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(abstractFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl(QString(ABSTRACT_URL).arg(m_Pmid))));
}

void PubMedDownloader::abstractFinished(QNetworkReply *reply)
{
    qWarning() << "PubMedDownloader Abstract" << reply->url();
    m_Abstract = reply->readAll();
    int b = m_Abstract.indexOf("<pre>\n1. ") + 9;
    int e = m_Abstract.indexOf("</pre>", b);
    m_Abstract.replace("&lt;", "<");
    m_Abstract.replace("&gt;", ">");
    m_Abstract = m_Abstract.mid(b, e-b);
    manager->disconnect();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(xmlFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl(QString(XML_URL).arg(m_Pmid))));
}

void PubMedDownloader::xmlFinished(QNetworkReply *reply)
{
    qWarning() << "PubMedDownloader xml" << reply->url();
    QDomDocument doc;
    QString content = reply->readAll();
    content.remove("<pre>");
    content.remove("</pre>");
    content.replace("&lt;","<");
    content.replace("&gt;",">");
    QString error;
    int line, col;
    if (!doc.setContent(content, &error, &line, &col)) {
        LOG_ERROR_FOR("PubMedDownloader", tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(error).arg(line).arg(col));
    }
    m_Xml = doc.toString(2);
    manager->disconnect();
    Q_EMIT downloadFinished();
}

