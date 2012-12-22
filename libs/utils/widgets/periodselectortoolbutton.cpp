/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "periodselectortoolbutton.h"

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>

#include <QMenu>
#include <QApplication>
#include <QStringList>

#include <QDebug>

using namespace Utils;
using namespace Trans::ConstantTranslations;

namespace Utils {
namespace Internal {
class PeriodSelectorToolButtonPrivate
{
public:
    PeriodSelectorToolButtonPrivate(PeriodSelectorToolButton *parent) :
        _periodMenu(0),
        _maxValue(10),
        _startPeriod(0),
        q(parent)
    {
    }

    QMenu *valueMenu(QMenu *parent, int period)
    {
        QMenu *valueMenu = new QMenu(parent);
        for(int i=1; i < _maxValue + 1; ++i) {
            QAction *a = valueMenu->addAction(QString::number(i));
            a->setData(period);
        }
        return valueMenu;
    }

    void populatePeriodMenu()
    {
        if (_periodMenu)
            return;
        _periodMenu = new QMenu(q);
        if (_trContext.isEmpty())
            _periodMenu->setTitle(_trTitle);
        else
            _periodMenu->setTitle(QApplication::translate(_trContext.toUtf8(), _trTitle.toUtf8()));
        for(int i = _startPeriod; i < periods().count(); ++i) {
            QAction *a = _periodMenu->addMenu(valueMenu(_periodMenu, i));
            a->setText(Utils::firstLetterUpperCase(periods().at(i)));
            a->setData(i); // store the period id
        }
        q->setMenu(_periodMenu);
    }

public:
    QMenu *_periodMenu;
    int _maxValue;
    int _startPeriod;
    QString _trTitle, _trContext;

private:
    PeriodSelectorToolButton *q;
};
}  // namespace Internal
}  // namespace Utils

PeriodSelectorToolButton::PeriodSelectorToolButton(QWidget *parent) :
    QToolButton(parent),
    d(new Internal::PeriodSelectorToolButtonPrivate(this))
{
    d->populatePeriodMenu();
    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(_actionTriggered(QAction*)));
}

PeriodSelectorToolButton::~PeriodSelectorToolButton()
{
    if (d)
        delete d;
    d = 0;
}

void PeriodSelectorToolButton::setMainMenuTitle(const QString &translatableTitle, const QString &translationContext)
{
    d->_trTitle = translatableTitle;
    d->_trContext = translationContext;
    if (d->_periodMenu)
        d->_periodMenu->setTitle(QApplication::translate(d->_trContext.toUtf8(), d->_trTitle.toUtf8()));
}

void PeriodSelectorToolButton::setStartPeriodsAt(const int transConstantsTimeEnumValue)
{
    d->_startPeriod = transConstantsTimeEnumValue;
    delete d->_periodMenu;
    d->_periodMenu = 0;
    d->populatePeriodMenu();
}

void PeriodSelectorToolButton::_actionTriggered(QAction *a)
{
    int period = a->data().toInt();
    int value = a->text().toInt();
    Q_EMIT periodSelected(period, value);
}

void PeriodSelectorToolButton::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        delete d->_periodMenu;
        d->_periodMenu = 0;
        d->populatePeriodMenu();
    }
    QToolButton::changeEvent(e);
}
