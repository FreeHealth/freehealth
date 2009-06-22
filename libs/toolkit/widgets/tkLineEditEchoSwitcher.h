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
#ifndef TKLINEEDITECHOSWITCHER_H
#define TKLINEEDITECHOSWITCHER_H
class tkLineEditEchoSwitcherPrivate;

#include <tkExporter.h>

/**
 * \file tkListView.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.7
 * \date 24 April 2009
*/

#include <QWidget>
#include <QLineEdit>
#include <QEvent>

/** \brief This widget is a set of two : one lineEdit and one pushButton. When button is clicked the echoMode of the lineEdit
    changes.
    Its property \e text assumes this widget to be inserted into QDataWidgetMapper.
  \ingroup toolkit
  \ingroup widget_toolkit
 */
class Q_TK_EXPORT tkLineEditEchoSwitcher : public QWidget
{
    Q_OBJECT
    /** */
    Q_PROPERTY( QString text READ text WRITE setText )

public:
    /** \brief Constructor */
    tkLineEditEchoSwitcher( QWidget *parent = 0 );
    ~tkLineEditEchoSwitcher() {}

    /** \brief return the line edit pointer. */
    QLineEdit *lineEdit();
    /** \brief Set the text of the line edit. */
    void setText( const QString & text );
    /** \brief Returns the line edit text. */
    QString text();

public Q_SLOTS:
    /** \brief Toogle the echo mode of the line edit. */
    void toogleEchoMode();

protected:
    /** */
    void changeEvent( QEvent *e );

private:
    tkLineEditEchoSwitcherPrivate *d;
};

#endif // TKLINEEDITECHOSWITCHER_H
