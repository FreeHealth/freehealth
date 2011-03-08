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
#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include <utils/global_exporter.h>

#include <QObject>

/**
 * \file messagesender.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.5
 * \date 09 Sept 2009
*/

namespace Utils {
namespace Constants {
const char * const URL_DOSAGETRANSMISSION     = "http://www.ericmaeker.fr/FreeMedForms/FMF_DosagesToStore.php";
const char * const URL_DRUGSARECORRECT        = "http://www.ericmaeker.fr/FreeMedForms/FMF_Correct.php";
const char * const URL_DRUGSAREINCORRECT      = "http://www.ericmaeker.fr/FreeMedForms/FMF_Uncorrect.php";
const char * const URL_DEVELOPPERMAILSENDING  = "http://www.ericmaeker.fr/FreeMedForms/FMF_Mail.php";
}  // End Constants

namespace Internal {
class MessageSenderPrivate;
}

class UTILS_EXPORT MessageSender : public QObject
{
    Q_OBJECT
public:
    enum typeOfMessage{
        CorrectDrugsCoding,
        UncorrectDrugsCoding,
        InformationToDevelopper,
        DosageTransmission
    };

    MessageSender( QObject *parent = 0 );
    ~MessageSender();

    // setters
    bool setTypeOfMessage( const typeOfMessage & t );
    void setParent( QWidget * parent );
    void setUser( const QString & usr);
    void setMessage( const QString & msg );
    void showResultingMessageBox( bool state );

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
    void httpDone( bool error );

private:
    Internal::MessageSenderPrivate *d;
};

}  // End Utils

#endif // MESSAGESENDER_H
