/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef BASEFORMWIDGET_H
#define BASEFORMWIDGET_H

//#include "basewigets_exporter.h"

#include <QWidget>
#include <QVariant>

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

/**
 * \file baseformwidgets.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 05 Apr 2010
*/

QT_BEGIN_NAMESPACE
class QGroupBox;
class QButtonGroup;
class QGridLayout;
class QCheckBox;
class QRadioButton;
class QLineEdit;
class QTextEdit;
class QListView;
class QComboBox;
class QDateTimeEdit;
class QAbstractSpinBox;
class QPushButton;
class QAbstractButton;
class QStringListModel;
QT_END_NAMESPACE

namespace BaseWidgets {
namespace Ui {
class BaseFormWidget;
}

class BaseWidgetsFactory : public Form::IFormWidgetFactory
{
    Q_OBJECT
public:
    BaseWidgetsFactory(QObject *parent = 0);
    ~BaseWidgetsFactory();

    bool initialize(const QStringList &arguments, QString *errorString);
    bool extensionInitialized();
    bool isInitialized() const;

    bool isContainer(const int idInStringList) const;
    QStringList providedWidgets() const;
    Form::IFormWidget *createWidget(const QString &name, Form::FormItem *object, QWidget *parent = 0);
};


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseForm implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseForm : public Form::IFormWidget
{
    Q_OBJECT
public:
    BaseForm(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~BaseForm();

    void addWidgetToContainer( Form::IFormWidget *widget );
    bool isContainer() const {return true;}

public Q_SLOTS:
    void retranslate();

public:
    QDateTimeEdit *m_EpisodeDate;
    QLineEdit *m_EpisodeLabel;
    QLabel *m_UserName;

private:
    QGridLayout *m_ContainerLayout;
    int i, row, col, numberColumns;
    Ui::BaseFormWidget *m_Header;
};

// Used to pass episode date, label, user...
class BaseFormData : public Form::IFormItemData
{
public:
    BaseFormData(Form::FormItem *item);
    ~BaseFormData();

    void setForm(BaseForm *form) {m_Form = form; clear();}
    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;

    // Use setData/Data for episode datas
    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    // No storable datas for forms
    void setStorableData(const QVariant &) {}
    QVariant storableData() const {return QVariant();}

private:
    Form::FormItem *m_FormItem;
    BaseForm *m_Form;
    QHash<int, QVariant> m_OriginalValue;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseGroup implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseGroup : public Form::IFormWidget
{
    Q_OBJECT
public:
    BaseGroup(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~BaseGroup();

    void addWidgetToContainer( Form::IFormWidget *widget );
    bool isContainer() const {return true;}

public Q_SLOTS:
    void retranslate();

private:
    QGroupBox *m_Group;
    QGridLayout *m_ContainerLayout;
    int i, row, col, numberColumns;

};


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseCheck implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseCheckData;
class BaseCheck : public Form::IFormWidget
{
    Q_OBJECT
public:
    BaseCheck(Form::FormItem *formItem, QWidget *parent = 0);
    ~BaseCheck();

public Q_SLOTS:
    void retranslate();

private:
    QCheckBox *m_Check;
    BaseCheckData *m_ItemData;
};

class BaseCheckData : public Form::IFormItemData
{
public:
    BaseCheckData(Form::FormItem *item);
    ~BaseCheckData();

    void setCheckBox(QCheckBox *chk);

    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;

    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    void setStorableData(const QVariant &data);
    QVariant storableData() const;

private:
    Form::FormItem *m_FormItem;
    QCheckBox *m_Check;
    Qt::CheckState m_OriginalValue;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseRadio implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseRadioData;
class BaseRadio : public Form::IFormWidget
{
    Q_OBJECT
public:
    BaseRadio(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~BaseRadio();

public Q_SLOTS:
    void retranslate();
    void buttonClicked(QAbstractButton *radio);

public:
    QList<QRadioButton*>  m_RadioList;
    QButtonGroup *m_ButGroup;
};

class BaseRadioData : public Form::IFormItemData
{
public:
    BaseRadioData(Form::FormItem *item);
    ~BaseRadioData();

    void setBaseRadio(BaseRadio* radio) {m_Radio = radio; clear();}

    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;

    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    void setStorableData(const QVariant &data);
    QVariant storableData() const;

private:
    Form::FormItem *m_FormItem;
    BaseRadio* m_Radio;
    QString m_OriginalValue;
};

//--------------------------------------------------------------------------------------------------------
//----------------------------------- BaseSimpleText implementation ------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseSimpleText : public Form::IFormWidget
{
     Q_OBJECT
public:
     BaseSimpleText(Form::FormItem *linkedObject, QWidget *parent = 0, bool shortText = true);
     ~BaseSimpleText();

public Q_SLOTS:
     void retranslate();

public:
     QLineEdit *m_Line;
     QTextEdit *m_Text;
};

class BaseSimpleTextData : public Form::IFormItemData
{
public:
    BaseSimpleTextData(Form::FormItem *item);
    ~BaseSimpleTextData();

    void setBaseSimpleText(BaseSimpleText* text) {m_Text = text; clear();}

    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;

    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    void setStorableData(const QVariant &data);
    QVariant storableData() const;

private:
    Form::FormItem *m_FormItem;
    BaseSimpleText* m_Text;
    QString m_OriginalValue;
};

//--------------------------------------------------------------------------------------------------------
//------------------------------------ BaseHelpText implementation -------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseHelpText : public Form::IFormWidget
{
     Q_OBJECT
public:
     BaseHelpText(Form::FormItem *linkedObject, QWidget *parent = 0);
     ~BaseHelpText();

public Q_SLOTS:
     void retranslate();
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseList implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseList : public Form::IFormWidget
{
     Q_OBJECT
public:
     BaseList(Form::FormItem *linkedObject, QWidget *parent = 0, bool uniqueList = true);
     ~BaseList();

public Q_SLOTS:
     void retranslate();

public:
     QListView *m_List;
     QStringListModel *m_Model;
};

class BaseListData : public Form::IFormItemData
{
public:
    BaseListData(Form::FormItem *item);
    ~BaseListData();

    void setBaseList(BaseList* list) {m_List = list; clear();}
    void setSelectedItems(const QString &s);

    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;

    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    void setStorableData(const QVariant &data);
    QVariant storableData() const;

private:
    Form::FormItem *m_FormItem;
    BaseList* m_List;
    QStringList m_OriginalValue;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseCombo implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseCombo : public Form::IFormWidget
{
     Q_OBJECT
public:
     BaseCombo(Form::FormItem *linkedObject, QWidget *parent = 0);
     ~BaseCombo();

public Q_SLOTS:
     void retranslate();

public:
     QComboBox *m_Combo;
};

class BaseComboData : public Form::IFormItemData
{
public:
    BaseComboData(Form::FormItem *item);
    ~BaseComboData();

    void setBaseCombo(BaseCombo* combo) {m_Combo = combo; clear();}
    int selectedItem(const QString &s);

    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;

    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    void setStorableData(const QVariant &data);
    QVariant storableData() const;

private:
    Form::FormItem *m_FormItem;
    BaseCombo* m_Combo;
    int m_OriginalValue;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseDate implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseDate : public Form::IFormWidget
{
     Q_OBJECT
public:
     BaseDate(Form::FormItem *linkedObject, QWidget *parent = 0);
     ~BaseDate();

 private Q_SLOTS:
     void onPatientChanged();

public Q_SLOTS:
     void retranslate();

public:
     QDateTimeEdit *m_Date;
};

class BaseDateData : public Form::IFormItemData
{
public:
    BaseDateData(Form::FormItem *item);
    ~BaseDateData();

    void setBaseDate(BaseDate* date) {m_Date = date; clear();}
    void setDate(const QString &s);

    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;

    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    void setStorableData(const QVariant &data);
    QVariant storableData() const;

private:
    Form::FormItem *m_FormItem;
    BaseDate* m_Date;
    QString m_OriginalValue;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseSpin implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseSpin : public Form::IFormWidget
{
     Q_OBJECT
public:
     BaseSpin(Form::FormItem *linkedObject, QWidget *parent = 0, bool doubleSpin = false);
     ~BaseSpin();

public Q_SLOTS:
     void retranslate();

public:
     QAbstractSpinBox *m_Spin;
};

class BaseSpinData : public Form::IFormItemData
{
public:
    BaseSpinData(Form::FormItem *item);
    ~BaseSpinData();

    void setBaseSpin(BaseSpin* spin) {m_Spin = spin; clear();}

    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;

    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    void setStorableData(const QVariant &data);
    QVariant storableData() const;

private:
    Form::FormItem *m_FormItem;
    BaseSpin* m_Spin;
    double m_OriginalValue;
};

//--------------------------------------------------------------------------------------------------------
//------------------------------------ BaseButton implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseButton : public Form::IFormWidget
{
     Q_OBJECT
public:
     BaseButton(Form::FormItem *linkedObject, QWidget *parent = 0);
     ~BaseButton();

public Q_SLOTS:
     void retranslate();

 private Q_SLOTS:
     void buttonClicked();
private:
     QPushButton *m_Button;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- SumWidget implementation ----------------------------------------
//--------------------------------------------------------------------------------------------------------
//class SumWidgetData;
class SumWidget : public Form::IFormWidget
{
    Q_OBJECT
public:
    SumWidget(Form::FormItem *formItem, QWidget *parent = 0);
    ~SumWidget();

private Q_SLOTS:
    void retranslate();
    void connectFormItems();
    void recalculate(const int modifiedRef);

private:
    QLineEdit *line;
//    SumWidgetData *m_ItemData;
};

//class SumWidgetData : public Form::IFormItemData
//{
//public:
//    BaseCheckData(Form::FormItem *item);
//    ~BaseCheckData();

//    void setCheckBox(QCheckBox *chk);

//    void clear();

//    Form::FormItem *parentItem() const {return m_FormItem;}
//    bool isModified() const;

//    bool setData(const int ref, const QVariant &data, const int role);
//    QVariant data(const int ref, const int role) const;

//    void setStorableData(const QVariant &data);
//    QVariant storableData() const;

//private:
//    Form::FormItem *m_FormItem;
//    QCheckBox *m_Check;
//    Qt::CheckState m_OriginalValue;
//};


} // End BaseWidgets

#endif
