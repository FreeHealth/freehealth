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
 *   Main developers : %AuthorName%                                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
@if "%Internal%" == "true"
#ifndef %PluginNamespace:u%_INTERNAL_%ClassName:u%_H
#define %PluginNamespace:u%_INTERNAL_%ClassName:u%_H
@else
#ifndef %PluginNamespace:u%_%ClassName:u%_H
#define %PluginNamespace:u%_%ClassName:u%_H
@endif

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

/**
 * \file %ClassName:l%.%CppHeaderSuffix:l%
 * \author %AuthorName%
 * \version 0.8.0
 * \date %CurrentDate%
*/

namespace %PluginNamespace:c% {
namespace Internal {
@if "%Internal%" != "true"
} // namespace Internal
@endif

class %ClassName:c% : public Form::IFormWidget
{
    Q_OBJECT
public:
    explicit %ClassName:c%(Form::FormItem *formItem, QWidget *parent = 0);
    ~%ClassName:c%();

    void addWidgetToContainer(Form::IFormWidget *widget);
    bool isContainer() const;

    // Printing
    QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
    void retranslate();

};

class %ClassName:c%Data : public Form::IFormItemData
{
    Q_OBJECT
public:
    %ClassName:c%Data(Form::FormItem *item, %ClassName:c% *formWidget);
    ~%ClassName:c%Data();
    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;
    void setModified(bool modified);

    void setReadOnly(bool readOnly);
    bool isReadOnly() const;

    // Use setData/Data for episode data
    bool setData(const int ref, const QVariant &data, const int role = Qt::EditRole);
    QVariant data(const int ref, const int role = Qt::DisplayRole) const;

    // Used storable data for forms
    void setStorableData(const QVariant &modified);
    QVariant storableData() const;

public Q_SLOTS:
    void onValueChanged();

private:
    Form::FormItem *m_FormItem;
    %ClassName:c% *m_Form;
    bool m_Modified;
    // m_OriginalData;
};


@if "%Internal%" == "true"
} // namespace Internal
@endif
} // namespace %PluginNamespace:c%

@if "%Internal%" == "true"
#endif // %PluginNamespace:u%_INTERNAL_%ClassName:u%_H
@else
#endif  // %PluginNamespace:u%_%ClassName:u%_H
@endif
