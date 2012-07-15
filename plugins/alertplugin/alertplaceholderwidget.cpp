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
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "alertplaceholdertest.h"
#include "alertitem.h"
#include "alertcore.h"
#include "nonblockingalertwidgets.h"
#include "alertitemeditordialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QAction>
#include <QWidget>
#include <QEvent>

#include <QDebug>

using namespace Alert;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Alert::AlertCore *alertCore() {return Alert::AlertCore::instance();}

AlertPlaceHolderTest::AlertPlaceHolderTest(QObject *parent) :
    IAlertPlaceHolder(parent),
    _widget(0),
    _newButton(0)
{
    setObjectName("AlertPlaceHolderTest");
}

AlertPlaceHolderTest::~AlertPlaceHolderTest()
{
    qWarning() << "AlertPlaceHolderTest::~AlertPlaceHolderTest()";
}

QString AlertPlaceHolderTest::uuid() const
{
    return "UID_TEST";
}

// for UI presentation of the place holder
QString AlertPlaceHolderTest::name(const QString &lang) const
{
    return "name";
}

QString AlertPlaceHolderTest::category(const QString &lang) const
{
    return "category";
}

QString AlertPlaceHolderTest::description(const QString &lang) const
{
    return "description";
}

void AlertPlaceHolderTest::clear()
{
    if (_widget)
        _widget->clear();
    alerts.clear();
    _buttons.clear();
    addNewAlertButton();
}

bool AlertPlaceHolderTest::addAlert(const AlertItem &alert)
{
    if (!containsAlertUuid(alert.uuid())) {
        if (_widget) {
            NonBlockingAlertToolButton *but = new NonBlockingAlertToolButton(_widget);
            but->setAlertItem(alert);

            // keep alert sorted by priority
            _priorities << alert.priority()*10000000 + alerts.count();
            alerts << alert;
            qSort(_priorities);

            int id = -1;
            bool br = false;
            for(int i = 0; i < _priorities.count(); ++i) {
                int prior = _priorities.at(i) / 10000000;

                if (alert.priority() < prior) {
                    id = _priorities.at(i);
                    br = true;
                } else if (alert.priority() == prior) {
                    id = _priorities.at(i);
                }
                if (br)
                    break;
            }

            if (id==-1) {
                _widget->addWidget(but);
            } else {
                QWidget *w = _buttons.value(alerts.at(id%10000000).uuid());
                QAction *before = 0;
                for(int i=0; i < _widget->actions().count(); ++i) {
                    if (_widget->widgetForAction(_widget->actions().at(i)) == w)
                        before = _widget->actions().at(i);
                }
                _widget->insertWidget(before, but);
            }
            _buttons.insert(alert.uuid(), but);
        }
    }
    return true;
}

bool AlertPlaceHolderTest::updateAlert(const AlertItem &alert)
{
//    qWarning() << "update Alert" << alert.label();
    if (containsAlertUuid(alert.uuid())) {
        // If alert is validated -> remove it
        // or update the content of the toolbutton
        if (alert.isUserValidated() || !alert.isValid())
            return removeAlert(alert);
        _buttons.value(alert.uuid())->setAlertItem(alert);
    } else {
        // add the alert
        return addAlert(alert);
    }
    return true;
}

bool AlertPlaceHolderTest::removeAlert(const AlertItem &alert)
{
//    qWarning() << "remove Alert" << alert.uuid();
    if (containsAlertUuid(alert.uuid())) {
        removeAlertUuid(alert.uuid());
        if (_widget) {
            QWidget *w = _buttons.value(alert.uuid());
            _buttons.remove(alert.uuid());
            for(int i=0; i < _widget->actions().count(); ++i) {
                if (_widget->widgetForAction(_widget->actions().at(i)) == w)
                    _widget->actions().at(i)->setVisible(false);
            }
        }
        _priorities.clear();
        for(int i = 0; i < alerts.count(); ++i) {
            _priorities << alerts.at(i).priority()*10000000 + i;
        }
        qSort(_priorities);
        if (_widget->sizePolicy().horizontalPolicy() != QSizePolicy::Expanding)
            _widget->adjustSize();
    }
    return true;
}

bool AlertPlaceHolderTest::highlightAlert(const AlertItem &alert)
{
    qWarning() << "highlighAlert" << alert.label();
    return true;
}

QWidget *AlertPlaceHolderTest::createWidget(QWidget *parent)
{
    if (!_widget) {
        _widget = new QToolBar(parent);
        _widget->setIconSize(QSize(16,16));
        addNewAlertButton();
    }
    for(int i = 0; i < alerts.count(); ++i) {
        addAlert(alerts[i]);
    }
    return _widget;
}

bool AlertPlaceHolderTest::containsAlert(const AlertItem &item)
{
    return alerts.contains(item);
}

bool AlertPlaceHolderTest::containsAlertUuid(const QString &alertUid)
{
    for(int i = 0; i < alerts.count(); ++i) {
        if (alerts.at(i).uuid() == alertUid)
            return true;
    }
    return false;
}

bool AlertPlaceHolderTest::removeAlertUuid(const QString &alertUid)
{
    for(int i = alerts.count()-1; i > -1 ; --i) {
        if (alerts.at(i).uuid() == alertUid)
            alerts.removeAt(i);
    }
    return false;
}

void AlertPlaceHolderTest::addNewAlertButton()
{
    if (!_newButton) {
        _newButton = new QToolButton(_widget);
        _newButton->setIconSize(QSize(16,16));
        _newButton->setIcon(theme()->icon(Core::Constants::ICONADD));
        _newButton->setText(tkTr(Trans::Constants::ADD_ALERT));
        _newButton->setToolTip(tkTr(Trans::Constants::ADD_ALERT));
        _newButton->installEventFilter(this);
        connect(_newButton, SIGNAL(clicked()), this, SLOT(createAlert()));
    }
    _widget->addWidget(_newButton);
    _widget->addSeparator();
}

void AlertPlaceHolderTest::createAlert()
{
    AlertItemEditorDialog dlg;
    dlg.setEditableParams(AlertItemEditorDialog::FullDescription | AlertItemEditorDialog::Types | AlertItemEditorDialog::Timing | AlertItemEditorDialog::Scripts);
    AlertItem item;
    item.setValidity(true);
    item.addTiming(AlertTiming(QDateTime::currentDateTime(), QDateTime::currentDateTime().addYears(1)));
    dlg.setAlertItem(item);
    if (dlg.exec()==QDialog::Accepted) {
         if (!dlg.submit(item)) {
             LOG_ERROR("Unable to submit alert");
         } else {
             alertCore()->saveAlert(item);
             alertCore()->registerAlert(item);
         }
    }
}

bool AlertPlaceHolderTest::eventFilter(QObject *obj, QEvent *event)
{
    if (obj==_newButton && event->type()==QEvent::LanguageChange) {
        _newButton->setText(tkTr(Trans::Constants::ADD_ALERT));
        _newButton->setToolTip(tkTr(Trans::Constants::ADD_ALERT));
        return true;
    }
    return false;
}
