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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric Maeker <eric.maeker@gmail.com>                 *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef BASEWIDGETS_TREEVIEWFORMITEM_H
#define BASEWIDGETS_TREEVIEWFORMITEM_H

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

QT_BEGIN_NAMESPACE
class QTreeView;
class QAbstractItemModel;
QT_END_NAMESPACE

/**
 * \file treeviewformitem.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 15 Feb 2013
*/

namespace BaseWidgets {

class TreeViewFormItem : public Form::IFormWidget
{
    Q_OBJECT
public:
    explicit TreeViewFormItem(Form::FormItem *formItem, QWidget *parent = 0);
    ~TreeViewFormItem();

    void addWidgetToContainer(Form::IFormWidget *widget);
    bool isContainer() const;

    // Printing
    QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
    void retranslate();

public:
    QTreeView *m_Tree;
    QAbstractItemModel *m_Model;
};

class TreeViewFormItemData : public Form::IFormItemData
{
    Q_OBJECT
public:
    TreeViewFormItemData(Form::FormItem *item, TreeViewFormItem *formWidget);
    ~TreeViewFormItemData();
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
    TreeViewFormItem *m_Form;
    bool m_Modified;
    // m_OriginalData;
};

} // namespace BaseWidgets

#endif  // BASEWIDGETS_TREEVIEWFORMITEM_H

