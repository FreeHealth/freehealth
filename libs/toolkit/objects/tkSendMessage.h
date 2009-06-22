/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
#ifndef TKSENDMESSAGE_H
#define TKSENDMESSAGE_H

#include <tkExporter.h>
class tkSendMessagePrivate;

#include <QObject>

/**
 * \file tkSendMessage.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.4
 * \date 01 May 2009
*/

Q_TK_BEGIN_CONSTANTS

const char * const URL_DRUGSARECORRECT = "http://www.ericmaeker.fr/FreeMedForms/FMF_Correct.php";
const char * const URL_DRUGSAREINCORRECT = "http://www.ericmaeker.fr/FreeMedForms/FMF_Uncorrect.php";
const char * const URL_DEVELOPPERMAILSENDING = "http://www.ericmaeker.fr/FreeMedForms/FMF_Mail.php";

Q_TK_END_CONSTANTS

/** \brief This class is a messagesender over internet.
  Change the enum and the Url inside member setTypeOfMessage().
  This class should not be destructed during the sending process.
  \ingroup toolkit
  \ingroup object_toolkit
*/
class Q_TK_EXPORT tkSendMessage : public QObject
{
    Q_OBJECT
public:

    enum typeOfMessage{
        CorrectDrugsCoding,
        UncorrectDrugsCoding,
        InformationToDevelopper
    };

    /** */
    tkSendMessage( QObject *parent = 0 );
    ~tkSendMessage();

    // setters
    /** \brief Defines the type of message. */
    bool setTypeOfMessage( const typeOfMessage & t );
    /** */
    void setParent( QWidget * parent );
    /** \brief Name of the user that is sending informations. */
    void setUser( const QString & usr);
    /** \brief Message to send. */
    void setMessage( const QString & msg );
    /** */
    void showResultingMessageBox( bool state );
    /** \brief The returned text of the server. */
    QString resultMessage();

    // getters
    /** */
    QString usedUrl();

    // starters
    /** \brief Starts the posting. */
    bool postMessage();

Q_SIGNALS:
    /** \brief signal emitted when sending is completed. */
    void sent();

private Q_SLOTS:
    void httpDone( bool error );

private:
    tkSendMessagePrivate *d;
};

#endif // TKSENDMESSAGE_H
