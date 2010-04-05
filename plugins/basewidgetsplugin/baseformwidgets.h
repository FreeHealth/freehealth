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
#ifndef MFBASEFORMWIDGET_H
#define MFBASEFORMWIDGET_H

//#include "basewigets_exporter.h"

#include <QWidget>
#include <QVariant>

#include <formmanagerplugin/iformwidgetfactory.h>

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
class QListWidget;
class QComboBox;
class QDateTimeEdit;
class QSpinBox;
class QPushButton;
QT_END_NAMESPACE

namespace BaseWidgets {

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

    void setValue(const QVariant &) {}
    QVariant value() const;

public Q_SLOTS:
    void retranslate();

private:
    QGridLayout *m_ContainerLayout;
    int i, row, col, numberColumns;
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

    void setValue(const QVariant &) {}
    QVariant value() const { return QVariant(); /** \todo this */ }

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
class BaseCheck : public Form::IFormWidget
{
    Q_OBJECT
public:
    BaseCheck(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~BaseCheck();

    void setValue(const QVariant &) {}
    QVariant value() const;

public Q_SLOTS:
    void retranslate();

//private Q_SLOTS:
//    void updateObject( bool chked );
//    void updateWidget();

private:
    QCheckBox *m_Check;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseRadio implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseRadio : public Form::IFormWidget
{
    Q_OBJECT
public:
    BaseRadio(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~BaseRadio();

    void setValue(const QVariant &) {}
    QVariant value() const {return QVariant();}

public Q_SLOTS:
    void retranslate();

    //private Q_SLOTS:
    //     void updateObject( bool chked );
    //     void updateWidget();

private:
    QList<QRadioButton*>  m_RadioList;
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

//private Q_SLOTS:
//     void updateObject( const QString & value );
//     void updateObject();
//     void updateWidget();

private:
     QLineEdit *m_Line;
     QTextEdit *m_Text;
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

//private Q_SLOTS:
//     void updateObject();
//     void updateWidget();
private:
     QListWidget *m_List;
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

//private Q_SLOTS:
//     void updateObject( int id );
//     void updateWidget();

private:
     QComboBox *m_Combo;
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

// private Q_SLOTS:
//     void updateObject( const QDateTime & datetime );
//     void updateObject( const QDate & date );
//     void updateWidget();

private:
     QDateTimeEdit *m_Date;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseSpin implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class BaseSpin : public Form::IFormWidget
{
     Q_OBJECT
public:
     BaseSpin(Form::FormItem *linkedObject, QWidget *parent = 0);
     ~BaseSpin();

public Q_SLOTS:
     void retranslate();

// private Q_SLOTS:
//     void updateObject( int val );
//     void updateWidget();
private:
     QSpinBox *m_Spin;
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
