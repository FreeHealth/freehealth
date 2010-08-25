/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
/**
  \class Utils::MessageSender
  \brief This class is a message sender over the web.
  It works with PHP scripts that are available on the FreeMedForms server.\n
  You can set the type of message you want to send using the enumerator MessageSender::typeOfMessage and setTypeOfMessage().\n
  You must set the message using setMessage(), optionnally set the user using setUser(), and the post the message using postMessage().\n
  Get the sending state using isSending().\n
  <b>The object should not be destructed during the sending process.</b>
*/

#include "messagesender.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QMessageBox>
#include <QHttp>
#include <QtNetwork>
#include <QBuffer>

using namespace Utils;
using namespace Utils::Internal;
using namespace Trans::ConstantTranslations;

namespace Utils {
namespace Internal {
class MessageSenderPrivate
{
public:
    MessageSenderPrivate() : m_Parent(0), m_Buffer(0), m_ShowMsgBox(false), m_IsSending(false) {}
    ~MessageSenderPrivate() { delete m_Buffer; }

    QUrl     url;
    QHttp    http;
    QWidget *m_Parent;
    QBuffer *m_Buffer;
    QString  m_User, m_Msg;
    bool     m_ShowMsgBox;
    QString  m_LastResult;
    bool     m_IsSending;
};
}
}


/** \brief Constructor */
MessageSender::MessageSender( QObject *parent )
             : QObject(parent), d(new MessageSenderPrivate())
{
    setObjectName( "MessageSender" );
    connect( &d->http, SIGNAL( done( bool ) ), this, SLOT( httpDone( bool ) ) );
}

/**
  \brief Destructor
  WARNING : Do not delete the object while sending the message
*/
MessageSender::~MessageSender()
{
    if (d) delete d;
    d=0;
}

/** \brief Set the parent QObject */
void MessageSender::setParent( QWidget * parent )
{ d->m_Parent = parent; }

/** \brief Set the user that is sending information */
void MessageSender::setUser( const QString & usr)
{ d->m_User = usr; }

/** \brief Set the the message to send */
void MessageSender::setMessage( const QString & msg )
{ d->m_Msg = msg ; }

/** \brief Return the currently used URL for the posting */
QString MessageSender::usedUrl() const
{ return d->url.toString(); }

/** \brief Returns the state of the transmission. */
bool MessageSender::isSending() const
{ return d->m_IsSending; }

/** \brief Set the result message box to shown or not at completion. */
void MessageSender::showResultingMessageBox( bool state )
{ d->m_ShowMsgBox = state; }

/** \brief The returned text of the server. */
QString MessageSender::resultMessage() const
{
    return d->m_LastResult;
}

/** \brief Set the type of message to send */
bool MessageSender::setTypeOfMessage( const typeOfMessage & t )
{
    switch (t)
    {
        case CorrectDrugsCoding   : d->url = QUrl(Constants::URL_DRUGSARECORRECT ); break;
        case UncorrectDrugsCoding : d->url = QUrl(Constants::URL_DRUGSAREINCORRECT ); break;
        case InformationToDevelopper : d->url = QUrl(Constants::URL_DEVELOPPERMAILSENDING ); break;
        case DosageTransmission : d->url = QUrl(Constants::URL_DOSAGETRANSMISSION); break;
        default : return false; break;
    }
    return true;
}

/** \brief Starts to post the message. Returns true if the sending has started. */
bool MessageSender::postMessage()
{
    if ( !d->url.isValid() )
        return false;
    if ( d->url.scheme() != "http" )
        return false;
    if ( d->url.path().isEmpty() )
        return false;
    if ( d->m_Msg.isEmpty() )
        return false;

    Utils::Log::addMessage( this, tkTr(Trans::Constants::STARTING_TASK_1).arg( tkTr(Trans::Constants::POST_TO_1).arg( d->url.toString() ) ) );

    d->http.setHost( d->url.host(), d->url.port( 80 ) );

    QHttpRequestHeader header( "POST", d->url.path() );
    header.setValue( "Host", d->url.host() );
    header.setContentType( "application/x-www-form-urlencoded" );

    QString s = "";
    if ( d->m_User.isEmpty() )
        s.append( "user=anomynous" );
    else
        s.append( "user=" + d->m_User );
    s.append( "&msg=" + d->m_Msg );

    d->http.setHost( d->url.host() );
    d->m_Buffer = new QBuffer( qApp );
    d->m_Buffer->open(QBuffer::ReadWrite);
    d->http.request( header, s.toUtf8(), d->m_Buffer );
    d->m_IsSending = true;
    return true;
}

/**
  \brief Slot connected to the http object.
  If the message is correctly sended a message box shows the result sended by the server (see showResultingMessageBox()). \n
  Then the signal sent() is emitted.
*/
void MessageSender::httpDone( bool error )
{
    QString ret = "";
    if ( !error ) {
        ret = tkTr(Trans::Constants::MESSAGE_SENDED_OK);
        Utils::Log::addMessage( this, ret );
    } else {
        ret = tkTr(Trans::Constants::ERROR_1_OCCURED_WHILE_2).arg(tkTr(Trans::Constants::POST_TO_1).arg(d->http.errorString()) );
        Utils::Log::addError( this, ret, __FILE__, __LINE__ );
        Utils::Log::addError( this, d->m_Buffer->data() , __FILE__, __LINE__);
    }

    d->m_LastResult = QString( d->m_Buffer->data() );

    // Show informations
    if ( d->m_ShowMsgBox ) {
        Utils::informativeMessageBox(ret , tkTr(Trans::Constants::INFORMATIVE_MESSAGE_1).arg(d->m_LastResult), "" );
    }

    if ( d->m_Buffer ) delete d->m_Buffer;
    d->m_Buffer = 0;
    d->m_IsSending = false;

    Q_EMIT sent();
}
