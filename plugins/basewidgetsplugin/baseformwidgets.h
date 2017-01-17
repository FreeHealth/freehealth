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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
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
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 15 Sept 2011
*/

QT_BEGIN_NAMESPACE
class QGroupBox;
class QButtonGroup;
class QToolButton;
class QGridLayout;
class QCheckBox;
class QRadioButton;
class QLineEdit;
class QTextEdit;
class QDateEdit;
class QDateTimeEdit;
class QAbstractSpinBox;
class QPushButton;
class QAbstractButton;
QT_END_NAMESPACE

namespace BaseWidgets {
namespace Internal {
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

    void createActions();
    int currentPriority() const;
    void setCurrentPriority(const int priority);

    void addWidgetToContainer(Form::IFormWidget *widget);
    bool isContainer() const {return true;}  // Always return true even if using QtUiFiles

    void showValidationMessage(const QString &message);
    void hideAndClearValidationMessage();

    // Printing
    QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
    void retranslate();

public:
    QDateTimeEdit *m_EpisodeDateTime;
    QLineEdit *m_EpisodeLabel;
    QToolButton *m_PriorityButton;

private:
    QGridLayout *m_ContainerLayout;
    int i, row, col, numberColumns;
    Ui::BaseFormWidget *ui;
    QAction *aScreenshot, *aHigh, *aMedium, *aLow;
};

// Used to pass episode date, label, user...
class BaseFormData : public Form::IFormItemData
{
public:
    BaseFormData(Form::FormItem *item);
    ~BaseFormData();

    void setForm(BaseForm *form);
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

private:
    Form::FormItem *m_FormItem;
    BaseForm *m_Form;
    QHash<int, QVariant> m_Data, m_OriginalData;
    bool m_Modified;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseGroup implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseGroupData;
class BaseGroup : public Form::IFormWidget
{
    Q_OBJECT
public:
    BaseGroup(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~BaseGroup();

    void addWidgetToContainer(Form::IFormWidget *widget);
    bool isContainer() const {return true;}

    // Printing
    QString printableHtml(bool withValues = true) const;

    void getCheckAndCollapsibleState();

public Q_SLOTS:
    void retranslate();
    void expandGroup(bool expand);
    void onToggled();

public:
    QGroupBox *m_Group;

private:
    BaseGroupData *m_ItemData;
    QGridLayout *m_ContainerLayout;
    int i, row, col, numberColumns;

};

class BaseGroupData : public Form::IFormItemData
{
    Q_OBJECT
public:
    BaseGroupData(Form::FormItem *item);
    ~BaseGroupData();

    void setBaseGroup(BaseGroup *gr);

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
    BaseGroup *m_BaseGroup;
    bool m_OriginalValue_isChecked;
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

    // Printing
    QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
    void retranslate();

private:
    QCheckBox *m_Check;
    BaseCheckData *m_ItemData;
};

class BaseCheckData : public Form::IFormItemData
{
    Q_OBJECT
public:
    BaseCheckData(Form::FormItem *item);
    ~BaseCheckData();

    void setCheckBox(QCheckBox *chk);

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

    QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
    void retranslate();
    void buttonClicked(QAbstractButton *radio);

public:
    QList<QRadioButton*>  m_RadioList;
    QButtonGroup *m_ButGroup;
};

class BaseRadioData : public Form::IFormItemData
{
    Q_OBJECT
public:
    BaseRadioData(Form::FormItem *item);
    ~BaseRadioData();

    void setBaseRadio(BaseRadio* radio) {m_Radio = radio; clear();}

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

     QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
     void retranslate();

public:
     QLineEdit *m_Line;
     QTextEdit *m_Text;
};

class BaseSimpleTextData : public Form::IFormItemData
{
    Q_OBJECT
public:
    BaseSimpleTextData(Form::FormItem *item);
    ~BaseSimpleTextData();

    void setBaseSimpleText(BaseSimpleText* text) {m_Text = text; clear();}

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

     QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
     void retranslate();
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

     QString printableHtml(bool withValues = true) const;

 private Q_SLOTS:
     void onCurrentPatientChanged();

public Q_SLOTS:
     void retranslate();

public:
     QDateEdit *m_Date;
};

class BaseDateData : public Form::IFormItemData
{
    Q_OBJECT
public:
    BaseDateData(Form::FormItem *item);
    ~BaseDateData();

    void setBaseDate(BaseDate* date) {m_Date = date; clear();}
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
    BaseDate* m_Date;
    QString m_OriginalValue;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseDateTime implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseDateTime : public Form::IFormWidget
{
     Q_OBJECT
public:
     BaseDateTime(Form::FormItem *linkedObject, QWidget *parent = 0);
     ~BaseDateTime();

     QString printableHtml(bool withValues = true) const;

 private Q_SLOTS:
     void onCurrentPatientChanged();

public Q_SLOTS:
     void retranslate();

public:
     QDateTimeEdit *m_DateTime;
};

class BaseDateTimeData : public Form::IFormItemData
{
    Q_OBJECT
public:
    BaseDateTimeData(Form::FormItem *item);
    ~BaseDateTimeData();

    void setBaseDateTime(BaseDateTime* dateTime) {m_DateTime = dateTime;}
    void setDateTime(const QString &s);

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
    BaseDateTime* m_DateTime;
    QDateTime m_OriginalDateTime;
    QString m_OriginalDateTimeValue;
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

     QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
     void retranslate();

public:
     QAbstractSpinBox *m_Spin;
};

class BaseSpinData : public Form::IFormItemData
{
    Q_OBJECT
public:
    BaseSpinData(Form::FormItem *item);
    ~BaseSpinData();

    void setBaseSpin(BaseSpin* spin) {m_Spin = spin; clear();}

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

}  // End namespace Internal
}  // End namespace BaseWidgets

#endif
