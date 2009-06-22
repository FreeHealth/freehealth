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
#include "tkLineEditEchoSwitcher.h"

#include <tkGlobal.h>
#include <tkTheme.h>

#include <QPushButton>
#include <QHBoxLayout>
#include <QApplication>

Q_TK_USING_CONSTANTS

class tkLineEditEchoSwitcherPrivate
{
public:
    tkLineEditEchoSwitcherPrivate( QWidget *parent ) : m_Parent(parent), toogler(0),lineEdit(0)
    {
        toogler = new QPushButton(parent);
        toogler->setIcon( tkTheme::icon(ICONEYES) );
        toogler->setSizePolicy( QSizePolicy::Fixed , QSizePolicy::Fixed );
        toogler->setFlat(true);
        toogler->setMaximumSize(QSize(22, 22));
        lineEdit = new QLineEdit(parent);
        lineEdit->setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Fixed );
    }

    void retranslate()
    {
        toogler->setToolTip( QCoreApplication::translate("tkLineEditEchoSwitcher","Display/Hide text") );
    }

    QWidget *m_Parent;
    QPushButton *toogler;
    QLineEdit *lineEdit;
};


tkLineEditEchoSwitcher::tkLineEditEchoSwitcher( QWidget *parent ) :
        QWidget(parent), d(0)
{
    setObjectName("tkLineEditEchoSwitcher");
    d = new tkLineEditEchoSwitcherPrivate(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(d->lineEdit);
    layout->addWidget(d->toogler);
    connect( d->toogler, SIGNAL(clicked()), this, SLOT(toogleEchoMode()));

}

void tkLineEditEchoSwitcher::setText( const QString & text )
{
    d->lineEdit->setText(text);
}

QString tkLineEditEchoSwitcher::text()
{
    return d->lineEdit->text();
}

void tkLineEditEchoSwitcher::toogleEchoMode()
{
    tkGlobal::switchEchoMode( d->lineEdit );
}

void tkLineEditEchoSwitcher::changeEvent( QEvent *e )
{
    Q_UNUSED(e);
    d->retranslate();
}

QLineEdit *tkLineEditEchoSwitcher::lineEdit()
{
    return d->lineEdit;
}

