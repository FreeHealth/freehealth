/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
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
    void setData(const QVariant &data, const int role);
    QVariant data(const int role) const;

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

    void setData(const QVariant &data, const int role);
    QVariant data(const int role) const;

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

public:
    QList<QRadioButton*>  m_RadioList;
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

    void setData(const QVariant &data, const int role);
    QVariant data(const int role) const;

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

    void setData(const QVariant &data, const int role);
    QVariant data(const int role) const;

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

    void setData(const QVariant &data, const int role);
    QVariant data(const int role) const;

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

    void setData(const QVariant &data, const int role);
    QVariant data(const int role) const;

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

    void setData(const QVariant &data, const int role);
    QVariant data(const int role) const;

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

    void setData(const QVariant &data, const int role);
    QVariant data(const int role) const;

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

} // End BaseWidgets

#endif
