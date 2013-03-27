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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef BASEWIDGETS_BASELISTWIDGETS_H
#define BASEWIDGETS_BASELISTWIDGETS_H

//#include "basewigets_exporter.h"

#include <QWidget>
#include <QVariant>

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

QT_BEGIN_NAMESPACE
class QListView;
class QComboBox;
class QStringListModel;
QT_END_NAMESPACE

/**
 * \file baselistwidgets.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.2
 * \date 01 Jan 2013
*/

namespace Views {
class StringListView;
}

namespace BaseWidgets {
namespace Internal {
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////   BaseList   ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BaseList : public Form::IFormWidget
{
     Q_OBJECT
public:
     BaseList(Form::FormItem *linkedObject, QWidget *parent = 0, bool uniqueList = true);
     ~BaseList();

     QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
     void retranslate();

public:
     QListView *m_List;
     QStringListModel *m_Model;
};

class BaseEditableStringList : public Form::IFormWidget
{
     Q_OBJECT
public:
     BaseEditableStringList(Form::FormItem *linkedObject, QWidget *parent = 0);
     ~BaseEditableStringList();

     QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
     void retranslate();

public:
     Views::StringListView *m_StringListView;
};

class BaseListData : public Form::IFormItemData
{
    Q_OBJECT
public:
    BaseListData(Form::FormItem *item);
    ~BaseListData();

    void setBaseList(BaseList* list) {m_List = list; clear();}
    void setBaseStringListView(BaseEditableStringList* list) {m_EditableList = list; clear();}
    void setSelectedItems(const QString &s);

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
    BaseList* m_List;
    BaseEditableStringList *m_EditableList;
    QStringList m_OriginalValue;
};

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////   BaseCombo   ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BaseCombo : public Form::IFormWidget
{
     Q_OBJECT
public:
     BaseCombo(Form::FormItem *linkedObject, QWidget *parent = 0);
     ~BaseCombo();

     QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
     void retranslate();

public:
     QComboBox *m_Combo;
};

class BaseComboData : public Form::IFormItemData
{
    Q_OBJECT
public:
    BaseComboData(Form::FormItem *item);
    ~BaseComboData();

    void setBaseCombo(BaseCombo* combo) {m_Combo = combo; clear();}
    int selectedItem(const QString &s);
    void setDefaultIndex(int index);
    int defaultIndex() const;

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
    BaseCombo* m_Combo;
    int m_OriginalValue;
    int m_DefaultIndex;
};

}  // End namespace Internal
}  // End namespace BaseWidgets

#endif // BASEWIDGETS_BASELISTWIDGETS_H
