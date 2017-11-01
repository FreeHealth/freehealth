/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef BASEWIDGETS_INTERNAL_BASEZIPCODESWIDGET_H
#define BASEWIDGETS_INTERNAL_BASEZIPCODESWIDGET_H

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

/**
 * \file basezipcodeswidgetwidget.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 07 Dec 2012
*/

namespace BaseWidgets {
namespace Internal {
class BaseDateCompleterData;

class BaseDateCompleterWidget : public Form::IFormWidget
{
    Q_OBJECT
    friend class BaseWidgets::Internal::BaseDateCompleterData;

public:
    BaseDateCompleterWidget(Form::FormItem *formItem, QWidget *parent = 0);
    ~BaseDateCompleterWidget();

    void addWidgetToContainer(Form::IFormWidget *widget);
    bool isContainer() const;

    // Printing
    QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
    void retranslate();

protected:
};

class BaseDateCompleterData : public Form::IFormItemData
{
    Q_OBJECT
public:
    BaseDateCompleterData(Form::FormItem *item);
    ~BaseDateCompleterData();

    void setBaseDate(BaseDateCompleterWidget* date) {m_Date = date; clear();}
    void setDate(const QString &s);

    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;
    void setModified(bool modified);

    void setReadOnly(bool readOnly);
    bool isReadOnly() const;

    bool setData(const int ref, const QVariant &data, const int role = Qt::EditRole);
    QVariant data(const int ref, const int role = Qt::DisplayRole) const;

    void setStorableData(const QVariant &data);
    QVariant storableData() const;

private Q_SLOTS:
    void onValueChanged();

private:
    Form::FormItem *m_FormItem;
    BaseDateCompleterWidget* m_Date;
    QString m_OriginalValue;
};

} // namespace Internal
} // namespace BaseWidgets

#endif // BASEWIDGETS_INTERNAL_BASEZIPCODESWIDGET_H
