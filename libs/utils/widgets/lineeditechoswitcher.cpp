/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Utils::LineEditEchoSwitcher
  Simple Widget with:
  - a QLineEdit
  - a simple button connected with the switchEchoMode() of the QLineEdit
  When using this widget please inform the icon of the button with LineEditEchoSwitcher::setIcon()
*/

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
        toogler->setFocusPolicy(Qt::ClickFocus);
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
    layout->setSpacing(0);
    layout->setMargin(0);
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

