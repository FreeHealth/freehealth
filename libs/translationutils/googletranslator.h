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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@free.fr>               *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef GOOGLETRANSLATOR_H
#define GOOGLETRANSLATOR_H

#include <translationutils/translationutils_exporter.h>
#include <QObject>
class QNetworkReply;
class QNetworkAccessManager;

namespace Utils {

class TRUTILS_EXPORT GoogleTranslator : public QObject
{
    Q_OBJECT
public:
    explicit GoogleTranslator(QObject *parent = 0);
    int setProxy(const QString &host, int port, const QString &username, const QString &password);

public Q_SLOTS:
    void replyFinished(QNetworkReply *reply);
    void textTranslated();
    void startTranslation(const QString &from,const QString &to, const QString &text);

Q_SIGNALS:
    void translationComplete(const QString &text);

private:
    QNetworkAccessManager *manager;

};

}  // End namespace Utils

#endif // GOOGLETRANSLATOR_H
