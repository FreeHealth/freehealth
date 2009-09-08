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
#include "lineeditechoswitcher.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QApplication>

using namespace Utils;
using namespace Utils::Internal;

namespace Utils {
namespace Internal {
class LineEditEchoSwitcherPrivate
{
public:
    LineEditEchoSwitcherPrivate( QWidget *parent ) : m_Parent(parent), toogler(0),lineEdit(0)
    {
        toogler = new QPushButton(parent);
//        toogler->setIcon( tkTheme::icon(ICONEYES) );
        toogler->setSizePolicy( QSizePolicy::Fixed , QSizePolicy::Fixed );
        toogler->setFlat(true);
        toogler->setMaximumSize(QSize(22, 22));
        lineEdit = new QLineEdit(parent);
        lineEdit->setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Fixed );
    }

    void retranslate()
    {
        toogler->setToolTip( QCoreApplication::translate("LineEditEchoSwitcher","Display/Hide text") );
    }

    void switchEchoMode( QLineEdit * l )
    {
        if ( l->echoMode() == QLineEdit::Normal )
            l->setEchoMode( QLineEdit::Password );
        else
            l->setEchoMode( QLineEdit::Normal );
    }


    QWidget *m_Parent;
    QPushButton *toogler;
    QLineEdit *lineEdit;
};
}  // End Internal
}  // End Utils

LineEditEchoSwitcher::LineEditEchoSwitcher(QWidget *parent) :
        QWidget(parent), d(0)
{
    setObjectName("LineEditEchoSwitcher");
    d = new LineEditEchoSwitcherPrivate(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(d->lineEdit);
    layout->addWidget(d->toogler);
    connect( d->toogler, SIGNAL(clicked()), this, SLOT(toogleEchoMode()));

}

    /** \brief Set the text of the line edit. */
void LineEditEchoSwitcher::setText( const QString & text )
{
    d->lineEdit->setText(text);
}

    /** \brief Returns the line edit text. */
QString LineEditEchoSwitcher::text()
{
    return d->lineEdit->text();
}

/** \brief Toogle the echo mode of the line edit. */
void LineEditEchoSwitcher::toogleEchoMode()
{
    d->switchEchoMode( d->lineEdit );
}

void LineEditEchoSwitcher::setIcon(const QIcon &icon)
{
    d->toogler->setIcon(icon);
}

void LineEditEchoSwitcher::changeEvent( QEvent *e )
{
    Q_UNUSED(e);
    d->retranslate();
}

    /** \brief return the line edit pointer. */
QLineEdit *LineEditEchoSwitcher::lineEdit()
{
    return d->lineEdit;
}

