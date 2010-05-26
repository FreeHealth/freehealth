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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef GIRWIDGET_H
#define GIRWIDGET_H

#include <aggirplugin/aggir_exporter.h>

#include <formmanagerplugin/iformwidgetfactory.h>

#include <QtGui/QWidget>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QAbstractButton;
QT_END_NAMESPACE

/**
 * \file girwidget.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 05 Apr 2010
*/


namespace Gir {
namespace Internal {

class GirWidgetFactory : public Form::IFormWidgetFactory
{
    Q_OBJECT
public:
    GirWidgetFactory(QObject *parent = 0);
    ~GirWidgetFactory() {}

    bool initialize(const QStringList &, QString *) {return true;}
    bool extensionInitialized() {return true;}
    bool isInitialized() const {return true;}

    bool isContainer(const int idInStringList) const {Q_UNUSED(idInStringList); return false;}
    QStringList providedWidgets() const {return QStringList() << "aggir" << "gir";}
    Form::IFormWidget *createWidget(const QString &name, Form::FormItem *object, QWidget *parent = 0);
};


//--------------------------------------------------------------------------------------------------------
//--------------------------------------- GirWidget implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
namespace Ui {
class GirWidget;
}
class GirUi : public QWidget
{
    Q_OBJECT
public:
    GirUi(QWidget *parent = 0);
    ~GirUi();

public Q_SLOTS:
    void updateGirString(QAbstractButton *radio);
    void on_aButton_clicked();
    void on_bButton_clicked();
    void on_cButton_clicked();

private:
    Ui::GirWidget *m_ui;
    QString m_GirString;
    int m_Gir;
};


class GirWidget : public Form::IFormWidget
{
    Q_OBJECT
public:
    GirWidget(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~GirWidget();

    bool isContainer() const {return false;}

    void setValue(const QVariant &);
    QVariant value() const;

public Q_SLOTS:
    void retranslate() {}

private:
};

} // End Internal
} // End Gir

#endif // GIRWIDGET_H
