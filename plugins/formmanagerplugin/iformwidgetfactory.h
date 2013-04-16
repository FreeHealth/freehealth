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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IFORMWIDGETFACTORY_H
#define IFORMWIDGETFACTORY_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <QFrame>
#include <QBoxLayout>
#include <QLabel>

/**
 * \file iformwidgetfactory.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 13 Sept 2011
*/

namespace Form {

class IFormWidget;
class FormItem;

class FORM_EXPORT IFormWidgetFactory : public QObject
{
    Q_OBJECT
public:
    IFormWidgetFactory(QObject *parent = 0) : QObject(parent) {}
    virtual ~IFormWidgetFactory() {}

    virtual bool initialize(const QStringList &arguments, QString *errorString) = 0;
    virtual bool extensionInitialized() = 0;
    virtual QStringList providedWidgets() const = 0;
    virtual bool isContainer( const int idInStringList ) const = 0;
    inline bool isContainer(const QString &name) {return isContainer(providedWidgets().indexOf(name)); }
    virtual bool isInitialized() const = 0;
    virtual IFormWidget *createWidget(const QString &name, Form::FormItem *linkedObject, QWidget *parent = 0) = 0;
};


class FORM_EXPORT IFormWidget : public QWidget
{
    Q_OBJECT
public:
    enum QFrameStyleForLabels {
        FormLabelFrame = QFrame::Panel | QFrame::Sunken,
        ItemLabelFrame = QFrame::NoFrame,
        HelpTextFrame =  QFrame::Panel | QFrame::Sunken
    };

    enum LabelOptions {
        NoOptions = 0,
        NoLabel,
        OnTop,
        OnBottom,
        OnLeft,
        OnRight,
        OnTopCentered
    };

    IFormWidget(Form::FormItem *linkedObject, QWidget *parent = 0);
    virtual ~IFormWidget() {}

    virtual void addWidgetToContainer(IFormWidget *) {}
    virtual bool isContainer() const {return false;}

    virtual void createLabel(const QString &text, Qt::Alignment horizAlign);
    virtual QBoxLayout *getBoxLayout(const int labelOption, const QString &text, QWidget *parent);

    virtual void changeEvent(QEvent *event);

    virtual void setFormItem(Form::FormItem *link) { m_FormItem = link; }
    virtual Form::FormItem *formItem() { return m_FormItem; }

    QWidget *focusableWidget() const {return _focusableWidget;}
    void setFocusableWidget(QWidget *widget) {_focusableWidget = widget;}

    QWidget *lastTabWidget() const {if (!_lastTabWidget) return _focusableWidget; return _lastTabWidget;}
    void setLastTabWidget(QWidget *widget) {_lastTabWidget = widget;}
    virtual void setTabOrder(bool consoleWarn = false) {Q_UNUSED(consoleWarn);}

    // Printing
    virtual QString printableHtml(bool withValues = true) const {Q_UNUSED(withValues); return QString();}

public Q_SLOTS:
    virtual void retranslate() {}

public:
    QLabel *m_Label;
    Form::FormItem *m_FormItem;
    QString m_OldTrans;
    QWidget *_focusableWidget, *_lastTabWidget;
};

} // namespace Form

#endif // IFORMWIDGETFACTORY_H
