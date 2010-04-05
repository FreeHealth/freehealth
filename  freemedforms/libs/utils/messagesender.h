/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
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
