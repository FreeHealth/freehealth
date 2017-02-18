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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include <utils/global_exporter.h>

#include <QObject>
QT_BEGIN_NAMESPACE
class QNetworkReply;
QT_END_NAMESPACE

/**
 * \file messagesender.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 17 Nov 2011
*/

namespace Utils {
namespace Internal {
class MessageSenderPrivate;
}

class UTILS_EXPORT MessageSender : public QObject
{
    Q_OBJECT
public:
    enum TypeOfMessage {
        CorrectDrugsCoding,
        UncorrectDrugsCoding,
        InformationToDeveloper,
        DosageTransmission
    };

    MessageSender(QObject *parent = 0);
    ~MessageSender();

    // setters
    bool setTypeOfMessage(const TypeOfMessage &t);
    void setParent(QWidget *parent);
    void setUser(const QString &usr);
    void setMessage(const QString &msg);
    void showResultingMessageBox(bool state);

    // getters
    QString resultMessage() const;
    QString usedUrl() const;
    bool isSending() const;

    // starters
    bool postMessage();

Q_SIGNALS:
    /** \brief signal emitted when sending is completed. */
    void sent();

private Q_SLOTS:
    void httpFinished(QNetworkReply*reply);

private:
    Internal::MessageSenderPrivate *d;
};

}  // End Utils

#endif // MESSAGESENDER_H
