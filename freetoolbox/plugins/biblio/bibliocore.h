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
#ifndef FREETOOLBOX_BIBLIO_CORE_H
#define FREETOOLBOX_BIBLIO_CORE_H

#include <biblio/biblio_exporter.h>
#include <QObject>
#include <QStringList>

/**
 * \file bibliocore.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 14 Oct 2011
*/

namespace Utils {
class PubMedDownloader;
}

namespace Biblio {
namespace Internal {
class BiblioPlugin;
class BiblioBase;
}

class BIBLIO_EXPORT BiblioCore : public QObject
{
    Q_OBJECT
    friend class Biblio::Internal::BiblioPlugin;

protected:
    explicit BiblioCore(QObject *parent = 0);

public:
    static BiblioCore *instance() {return m_Instance;}

    void downloadPubMedData(const QStringList &pmids);

    QString xml(const QString &pmid);

private Q_SLOTS:
    void downloadNext();

private:
    static BiblioCore *m_Instance;
    Internal::BiblioBase *m_Base;
    Utils::PubMedDownloader *m_dld;
    QStringList m_ToDownload;
};

}  // End namespace Biblio

#endif // FREETOOLBOX_BIBLIO_CORE_H
