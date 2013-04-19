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
 ***************************************************************************/
#ifndef IDENTITYWIDGETFACTORY_H
#define IDENTITYWIDGETFACTORY_H

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

#include <QStringList>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file identitywidgetfactory.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 12 Dec 2012
*/

namespace Identity {
class IdentityEditorWidget;
}

namespace BaseWidgets {

class IdentityWidgetFactory : public Form::IFormWidgetFactory
{
    Q_OBJECT
public:
    IdentityWidgetFactory(QObject *parent = 0);
    ~IdentityWidgetFactory();

    bool initialize(const QStringList &arguments, QString *errorString);
    bool extensionInitialized();
    bool isInitialized() const;

    bool isContainer(const int idInStringList) const;
    QStringList providedWidgets() const;
    Form::IFormWidget *createWidget(const QString &name, Form::FormItem *object, QWidget *parent = 0);
};

class IdentityFormWidget : public Form::IFormWidget
{
    Q_OBJECT
public:
    IdentityFormWidget(Form::FormItem *formItem, QWidget *parent = 0);
    ~IdentityFormWidget();

    QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
    void retranslate();

public:
    Identity::IdentityEditorWidget *m_Identity;

private:
    QGridLayout *m_ContainerLayout;
};

class IdentityWidgetData : public Form::IFormItemData
{
public:
    IdentityWidgetData(Form::FormItem *item) :
        m_FormItem(item),
        m_Widget(0)
    {}

    ~IdentityWidgetData() {}

    void setIdentityFormWiget(IdentityFormWidget *widget) {m_Widget = widget; clear();}
    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;
    void setModified(bool modified);

    void setReadOnly(bool readOnly);
    bool isReadOnly() const;

    // Use setData/Data for episode data
    bool setData(const int ref, const QVariant &data, const int role) {Q_UNUSED(ref); Q_UNUSED(data); Q_UNUSED(role); return true;}
    QVariant data(const int ref, const int role) const;

    // No storable data for forms
    void setStorableData(const QVariant &value);
    QVariant storableData() const;

private:
    Form::FormItem *m_FormItem;
    IdentityFormWidget *m_Widget;
};

}  // End namespace BaseWidgets

#endif // IDENTITYWIDGETFACTORY_H
