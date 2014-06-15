/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2014 by Eric MAEKER, MD                                      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRCPLUGIN_INTERNAL_EDRCWIDGETFACTORY_H
#define EDRCPLUGIN_INTERNAL_EDRCWIDGETFACTORY_H

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

#include <QWidget>
QT_BEGIN_NAMESPACE
class QModelIndex;
class QPushButton;
QT_END_NAMESPACE

/**
 * \file edrcwidgetfactory.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.9.2
 * \date 06 Oct 2013
*/

namespace eDRC {
namespace Internal {
class CrListViewer;
class CrTreeModel;

class EdrcWidgetFactory : public Form::IFormWidgetFactory
{
    Q_OBJECT
public:
    EdrcWidgetFactory(QObject *parent = 0);
    ~EdrcWidgetFactory();

    bool initialize(const QStringList &arguments, QString *errorString);
    bool extensionInitialized();

    bool isContainer( const int idInStringList ) const;
    QStringList providedWidgets() const;
    bool isInitialized() const { return true; }

    Form::IFormWidget *createWidget(const QString &name, Form::FormItem *linkedObject, QWidget *parent = 0);
};


//--------------------------------------------------------------------------------------------------------
//------------------------------------  DrugsWidget implementation  --------------------------------------
//--------------------------------------------------------------------------------------------------------
class EdrcWidget : public Form::IFormWidget
{
    Q_OBJECT
public:
    EdrcWidget(const QString &name, Form::FormItem *linkedObject, QWidget *parent);
    ~EdrcWidget();

    QString printableHtml(bool withValues = true) const;

    void retranslate();

//private Q_SLOTS:
//    void addChronicTherapeutics();

public:
    CrListViewer *m_CentralWidget;
    CrTreeModel *m_CrTreeModel;
//private:
//    QString m_iniPath;
//    bool m_WithPrescribing, m_WithPrinting;
//    QPushButton *m_AddChronic;
};

//---------------------------------------------------------------------------------------
//--------------------------   EdrcWidgetData implementation   --------------------------
//---------------------------------------------------------------------------------------
class EdrcWidgetData : public Form::IFormItemData
{
public:
    EdrcWidgetData(Form::FormItem *item);
    ~EdrcWidgetData();

    void setEdrcWidget(EdrcWidget *widget);

    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;
    void setModified(bool modified);

    void setReadOnly(bool readOnly);
    bool isReadOnly() const;

    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    void setStorableData(const QVariant &data);
    QVariant storableData() const;

private:
    Form::FormItem *m_FormItem;
    EdrcWidget *m_Widget;
    QString m_OriginalValue;
};

}  // End Internal
}  // End eDRC

#endif  // EDRCPLUGIN_INTERNAL_EDRCWIDGETFACTORY_H
