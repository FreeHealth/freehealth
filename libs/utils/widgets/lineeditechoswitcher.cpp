/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
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
#include <QToolButton>

using namespace Utils;
using namespace Utils::Internal;

namespace Utils {
namespace Internal {
class LineEditEchoSwitcherPrivate
{
public:
    LineEditEchoSwitcherPrivate() :
        m_ToolButton(0)
    {
    }

    void switchEchoMode(QLineEdit *l)
    {
        if (l->echoMode() == QLineEdit::Normal)
            l->setEchoMode(QLineEdit::Password);
        else
            l->setEchoMode(QLineEdit::Normal);
    }

    QToolButton *m_ToolButton;
};
}  // End Internal
}  // End Utils

LineEditEchoSwitcher::LineEditEchoSwitcher(QWidget *parent) :
    QButtonLineEdit(parent),
    d(0)
{
    setObjectName("LineEditEchoSwitcher");
    d = new LineEditEchoSwitcherPrivate;
    d->m_ToolButton = new QToolButton(this);
    d->m_ToolButton->setToolTip(QCoreApplication::translate("LineEditEchoSwitcher","Display/Hide text"));
    d->m_ToolButton->setFocusPolicy(Qt::ClickFocus);
    setRightButton(d->m_ToolButton);
    connect(d->m_ToolButton, SIGNAL(clicked()), this, SLOT(toogleEchoMode()));
}

LineEditEchoSwitcher::~LineEditEchoSwitcher()
{
    if (d) {
        delete d;
        d = 0;
    }
}


/** \brief Toogle the echo mode of the line edit. */
void LineEditEchoSwitcher::toogleEchoMode()
{
    d->switchEchoMode(this);
}

void LineEditEchoSwitcher::setIcon(const QIcon &icon)
{
    d->m_ToolButton->setIcon(icon);
}

void LineEditEchoSwitcher::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        d->m_ToolButton->setToolTip(QCoreApplication::translate("LineEditEchoSwitcher","Display/Hide text"));
    }
}
