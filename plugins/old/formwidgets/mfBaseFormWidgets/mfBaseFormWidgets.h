/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MFBASEFORMWIDGET_H
#define MFBASEFORMWIDGET_H

#include <QtGui>
#include <QWidget>

#include <mfObject.h>
#include <mfFormWidgetInterface.h>
#include <mfAbstractWidget.h>

/**
 * \file mfBaseFormWidgets.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.01
 * \date 01 novembre 2008
*/

class mfBaseFormWidgets : public mfFormWidgetInterface
{
     Q_OBJECT
     Q_INTERFACES( mfBaseInterface mfFormWidgetInterface );

public:
     mfBaseFormWidgets();

     QStringList   widgets() const;
     bool          isContainer( const int idInStringList ) const;
     mfAbstractWidget * getWidget( mfObject * mfo, mfAbstractWidget * parent = 0 ) const;
};


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfBaseForm implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class mfBaseForm : public mfAbstractWidget
{
     Q_OBJECT
public:
     mfBaseForm( mfObject * mfo, mfAbstractWidget * parent );
     ~mfBaseForm();

     void addWidgetToContainer( mfAbstractWidget * widget );
public slots:
     void retranslateUi( const QString & );

private:
     QGridLayout * m_ContainerLayout;
     int i, row, col, numberColumns;

};


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfBaseGroup implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
class mfBaseGroup : public mfAbstractWidget
{
     Q_OBJECT
public:
     mfBaseGroup( mfObject * mfo, mfAbstractWidget * parent );
     ~mfBaseGroup();

     void addWidgetToContainer( mfAbstractWidget * widget );
public slots:
     void retranslateUi( const QString & );

private slots:
     void updateObject( bool );
     void updateWidget();

private:
     QGroupBox * m_Group;
     QGridLayout * m_ContainerLayout;
     int i, row, col, numberColumns;

};


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfBaseCheck implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
class mfBaseCheck : public mfAbstractWidget
{
     Q_OBJECT
public:
     mfBaseCheck( mfObject * mfo, mfAbstractWidget * parent );
     ~mfBaseCheck();

public slots:
     void retranslateUi( const QString & );

private slots:
     void updateObject( bool chked );
     void updateWidget();

private:
     QCheckBox * m_Check;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfBaseRadio implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
class mfBaseRadio : public mfAbstractWidget
{
     Q_OBJECT
public:
     mfBaseRadio( mfObject * mfo, mfAbstractWidget * parent );
     ~mfBaseRadio();

public slots:
     void retranslateUi( const QString & );

private slots:
     void updateObject( bool chked );
     void updateWidget();

private:
     QList<QRadioButton*>  m_RadioList;
};

//--------------------------------------------------------------------------------------------------------
//----------------------------------- mfBaseSimpleText implementation ------------------------------------
//--------------------------------------------------------------------------------------------------------
class mfBaseSimpleText : public mfAbstractWidget
{
     Q_OBJECT
public:
     mfBaseSimpleText( mfObject * mfo, mfAbstractWidget * parent );
     ~mfBaseSimpleText();

public slots:
     void retranslateUi( const QString & );

private slots:
     void updateObject( const QString & value );
     void updateObject();
     void updateWidget();

private:
     QLineEdit *     m_Line;
     QTextEdit *     m_Text;
};

//--------------------------------------------------------------------------------------------------------
//------------------------------------ mfBaseHelpText implementation -------------------------------------
//--------------------------------------------------------------------------------------------------------
class mfBaseHelpText : public mfAbstractWidget
{
     Q_OBJECT
public:
     mfBaseHelpText( mfObject * mfo, mfAbstractWidget * parent );
     ~mfBaseHelpText();

public slots:
     void retranslateUi( const QString & );
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfBaseList implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class mfBaseList : public mfAbstractWidget
{
     Q_OBJECT
public:
     mfBaseList( mfObject * mfo, mfAbstractWidget * parent );
     ~mfBaseList();

public slots:
     void retranslateUi( const QString & );
private slots:
     void updateObject();
     void updateWidget();
private:
     QListWidget *   m_List;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfBaseCombo implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
class mfBaseCombo : public mfAbstractWidget
{
     Q_OBJECT
public:
     mfBaseCombo( mfObject * mfo, mfAbstractWidget * parent );
     ~mfBaseCombo();

public slots:
     void retranslateUi( const QString & );
private slots:
     void updateObject( int id );
     void updateWidget();

private:
     QComboBox *     m_Combo;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfBaseDate implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class mfBaseDate : public mfAbstractWidget
{
     Q_OBJECT
public:
     mfBaseDate( mfObject * mfo, mfAbstractWidget * parent );
     ~mfBaseDate();

public slots:
     void retranslateUi( const QString & );
private slots:
     void updateObject( const QDateTime & datetime );
     void updateObject( const QDate & date );
     void updateWidget();

private:
     QDateTimeEdit * m_Date;
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfBaseSpin implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class mfBaseSpin : public mfAbstractWidget
{
     Q_OBJECT
public:
     mfBaseSpin( mfObject * mfo, mfAbstractWidget * parent );
     ~mfBaseSpin();

public slots:
     void retranslateUi( const QString & );
private slots:
     void updateObject( int val );
     void updateWidget();
private:
     QSpinBox *      m_Spin;
};

//--------------------------------------------------------------------------------------------------------
//------------------------------------ mfBaseButton implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
class mfBaseButton : public mfAbstractWidget
{
     Q_OBJECT
public:
     mfBaseButton( mfObject * mfo, mfAbstractWidget * parent );
     ~mfBaseButton();

public slots:
     void retranslateUi( const QString & );
private slots:
     void buttonClicked();
private:
     QPushButton *   m_Button;
};

#endif
