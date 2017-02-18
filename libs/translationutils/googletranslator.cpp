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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Utils::GoogleTranslator
  Small connector to the Google API for translation.
*/

#include "googletranslator.h"

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>
#include <QUrlQuery>

using namespace Utils;

/** Create a Google translator object. */
GoogleTranslator::GoogleTranslator(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);
//    setProxy("chaisa1", 8080, "urg", "debut1");
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

/** Define the proxy settings. */
int GoogleTranslator::setProxy(const QString & host, int port, const QString & username , const QString & password )
{
    QNetworkProxy prox(QNetworkProxy::HttpProxy, host, port, username, password);
    manager->setProxy(prox);
    return 0;
}

/**
  Starts a translation from the language \e from to the language \e to of the \e text.
  You can define an unique identifier for your request. Use the \e uid param.\n
  When the translation is done, a signal is emitted translationComplete() with the translated string and
  the translationCompleteWithUid() with the identifier of the translation.
*/
void GoogleTranslator::startTranslation(const QString &from, const QString &to, const QString &text, const QString &uid)
{
    QUrl url("http://ajax.googleapis.com/", QUrl::TolerantMode);
    QUrlQuery query;
    url.setPath("/ajax/services/language/translate");
    query.addQueryItem("v","1.0");
    query.addQueryItem("q", text);
    query.addQueryItem("langpair",QString("%1|%2").arg(from).arg(to).toUtf8());
    url.setQuery(query);
    if (!uid.isEmpty())
        uidToUrl.insert(uid, url);
    manager->get(QNetworkRequest(url));
}

void GoogleTranslator::replyFinished(QNetworkReply *reply)
{
//    qWarning() << "reply" << reply->isFinished() << reply->isReadable() << reply->errorString();
    QString text = QString::fromUtf8(reply->readAll());
//    qWarning() << "Google"<<text;
    int start = text.indexOf("translatedText\":")+17;
    int end = text.indexOf("\"}, \"", start);
    text = text.mid(start, end - start);
    text.replace("\\u0026gt; \\u003d", "≥");
    text.replace("\\u0026gt;", ">");
    text.replace("\\u0026lt;", "<");
    text.replace("\\u0026lt; \\u003d", "≤");
    text.replace("\\u003d", "=");
    text.replace("\\u003cbr\\u003e","<br />");
    text.replace(QString("\\x3c"), "<");
    text.replace(QString("\\x3e"), ">");
    text.replace(QString("\\u003c"),QString("<"));
    text.replace(QString("\\u003e"),QString(">"));
    Q_EMIT translationComplete(text);
    if (uidToUrl.values().contains(reply->url())) {
        Q_EMIT translationCompleteWithUid(text, uidToUrl.key(reply->url()));
        uidToUrl.remove(uidToUrl.key(reply->url()));
    }
}

void GoogleTranslator::textTranslated()
{
    QString text;
//    text = text.fromUtf8( http->readAll() );

    text = text.replace(QString("\\\""),QString("\""));
    text = text.replace(QString("\\n"),QString("\n"));
    text = text.replace(QString("\n "),QString("\n"));
    text = text.replace(QString("\\x3c"),QString("<"));
    text = text.replace(QString("\\u003c"),QString("<"));
    text = text.replace(QString("\\x3e"),QString(">"));
    text = text.replace(QString("\\u003e"),QString(">"));

    if (text.startsWith(QString("\""))) {
        // This is a text
        text = text.remove( text.length()-1, 1).remove(0,1);
    } else if (text.startsWith( QString("[")) && text.endsWith(QString("]"))) {
        // This is a word
        // Need dictionary mode
        QStringList tra;
        //tra = text.split(QRegExp(QString("\"(.*)\"")));
        text = text.replace(QString("]"),QString(""));
        tra = text.split(QString("["));
        text = QString("");
        for(int i=0,j=0;i<tra.count();i++) {
            if(tra.at(i)!="") {
                if(j==0) {
                    QString translation = tra.at(i);
                    translation = translation.replace("\"","");
                    translation = translation.replace(",","");
                    text.append( translation );
                    text.append( "\n\n") ;
                } else {
                    QString translation = tra.at(i);
                    QStringList translations = translation.split(",");
                    for(int y=0;y<translations.count();y++) {
                        translation = translations.at(y);
                        translation = translation.replace("\"","");
                        if(y==0) {
                            text.append(QString(translation + ": "));
                        } else {
                            text.append(QString("\t" + translation + "\n" ));
                        }
                    }
                    text.append( "\n" );
                }
                j++;
            }
        }
    }
    Q_EMIT(translationComplete(text));
}
