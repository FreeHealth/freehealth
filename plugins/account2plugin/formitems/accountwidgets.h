/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric Maeker <eric.maeker@gmail.com>                 *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ACCOUNT_INTERNAL_ACCOUNTWIDGETS_H
#define ACCOUNT_INTERNAL_ACCOUNTWIDGETS_H

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

/**
 * \file accountwidgets.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 16 Mar 2013
*/

namespace Account2 {
namespace Internal {

class AccountWidgetFactory : public Form::IFormWidgetFactory
{
    Q_OBJECT
public:
    AccountWidgetFactory(QObject *parent = 0);
    ~AccountWidgetFactory();

    bool initialize(const QStringList &arguments, QString *errorString);
    bool extensionInitialized();
    bool isInitialized() const;

    bool isContainer(const int) const;
    QStringList providedWidgets() const;
    Form::IFormWidget *createWidget(const QString &name, Form::FormItem *object, QWidget *parent = 0);
};

class FeeFormWidget : public Form::IFormWidget
{
    Q_OBJECT

public:
    explicit FeeFormWidget(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~FeeFormWidget();

    void addWidgetToContainer(Form::IFormWidget *widget);
    bool isContainer() const;

    // Printing
    QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
    void retranslate();

};

class FeeFormWidgetData : public Form::IFormItemData
{
public:
    FeeFormWidgetData(Form::FormItem *item, FeeFormWidget *formWidget);
    ~FeeFormWidgetData();
    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;
    void setModified(bool modified);

    // Use setData/Data for episode data
    bool setData(const int ref, const QVariant &data, const int role = Qt::EditRole);
    QVariant data(const int ref, const int role = Qt::DisplayRole) const;

    // Used storable data for forms
    void setStorableData(const QVariant &modified);
    QVariant storableData() const;

//private Q_SLOTS:
//    void onValueChanged();

private:
    Form::FormItem *m_FormItem;
    FeeFormWidget *m_Form;
    bool m_Modified;
    // m_OriginalData;
};


} // namespace Internal
} // namespace Account2

#endif // ACCOUNT_INTERNAL_ACCOUNTWIDGETS_H

