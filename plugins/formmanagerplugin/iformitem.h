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
#ifndef IFORMITEM_H
#define IFORMITEM_H

#include <coreplugin/idebugpage.h>
#include <coreplugin/constants.h>

#include <formmanagerplugin/formmanager_exporter.h>
#include <formmanagerplugin/iformitemspec.h>
#include <formmanagerplugin/iformitemscripts.h>
#include <formmanagerplugin/iformitemvalues.h>

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QWidget>
#include <QVariant>
#include <QPointer>
#include <QHash>


#include <QDebug>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE


/**
 * \file iformitem.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 05 Apr 2010
*/

namespace Form {
class IFormWidget;
class IFormItemData;
class FormItemIdentifiants;
class FormPage;
class FormMain;
class FormMainDebugPage;


/**
 \class FormItem : bas niveau dans la définition de l'item
- paramètres d'information (date de création, auteur, version, licence...)
- définit en valeurs uniques ou non (par ex identité == valeurs uniques)
- Options d'affichage et d'impression ??
- Options de "droit" / utilisateurs autorisés
- Scripts (OnLoad, PostLoad, CallMe)
- Base de règles
- Plugin associé
- 1 QWidget vue (les widgets sont construits par les IFormIO selon leur spécificités)
- 1 QWidget historique
- checkValueIntegrity() qui se base sur les règles de l'item pour vérifier son exactitude
*/
class FORM_EXPORT FormItem : public Form::FormItemIdentifiants
{
    Q_OBJECT
//    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)

public:
    FormItem(QObject *parent=0) :
            FormItemIdentifiants(parent),
            m_Spec(new FormItemSpec),
            m_Scripts(new FormItemScripts),
            m_Values(new FormItemValues(this)),
            m_ItemDatas(0)
            {}

    virtual ~FormItem();

    Form::FormItemSpec *spec() const {return m_Spec;}
    Form::FormItemScripts *scripts() const {return m_Scripts;}
    Form::FormItemValues *valueReferences() const {return m_Values;}

    // Access to database values. Pointer will not be deleted
    void setItemDatas(Form::IFormItemData *data) {m_ItemDatas = data;}
    Form::IFormItemData *itemDatas() {return m_ItemDatas;}

    // Access to the user's widget
    virtual void setFormWidget(Form::IFormWidget *w) {m_FormWidget=w;}
    virtual IFormWidget *formWidget() {return m_FormWidget;}

    // Access to the FormItem tree
    virtual FormItem *createChildItem(const QString &uuid = QString::null);
    virtual FormPage *createPage(const QString &uuid = QString::null) {Q_UNUSED(uuid); return 0;}
    virtual QList<FormItem*> formItemChildren() const;

    // FormIO extra datas
    virtual void addExtraData(const QString &id, const QString &data);
    virtual QHash<QString,QString> extraDatas() const {return m_ExtraDatas;}
    virtual void clearExtraDatas() {m_ExtraDatas.clear();}

public Q_SLOTS:
    virtual void languageChanged();

private:
    Form::FormItemSpec *m_Spec;
    Form::FormItemScripts *m_Scripts;
    Form::FormItemValues *m_Values;
    Form::IFormWidget *m_FormWidget;
    Form::IFormItemData *m_ItemDatas;
    QHash<QString, QString> m_ExtraDatas;
};
inline QList<Form::FormItem*> Form::FormItem::formItemChildren() const
{
     QList<Form::FormItem*> list;
     foreach(QObject *o, children()) {
          Form::FormItem *i = qobject_cast<Form::FormItem*>(o);
          if (i) {
              list.append(i);
          }
     }
     return list;
}


class FORM_EXPORT FormPage : public FormItem
{
    Q_OBJECT
public:
    FormPage(QObject *parent=0) :
            FormItem(parent) {}
    ~FormPage() {}

    virtual FormPage *createPage(const QString &uuid = QString::null) {Q_UNUSED(uuid); return 0;}

    virtual void languageChanged();

private:
};

class FORM_EXPORT FormMain : public FormItem
{
    Q_OBJECT
public:
    FormMain(QObject *parent=0) :
            FormItem(parent), m_DebugPage(0), m_Multi(true) {}
    ~FormMain();

    FormPage *createPage(const QString &uuid = QString::null);

    virtual void languageChanged();
    void clear();

    virtual FormMain *formParent() { return qobject_cast<FormMain*>(parent()); }

    virtual void setMultiEpisode(bool state) {m_Multi = state;}
    virtual bool isMultiEpisode() const {return m_Multi;}

    virtual void setUniqueEpisode(bool state) {m_Multi = !state;}
    virtual bool isUniqueEpisode() const {return !m_Multi;}

    void createDebugPage();
    void toTreeWidget(QTreeWidget *tree);

private:
    FormMainDebugPage *m_DebugPage;
    bool m_Multi;
};

class FORM_EXPORT FormMainDebugPage : public Core::IDebugPage
{
    Q_OBJECT
public:
    FormMainDebugPage(FormMain *form, QObject *parent);
    ~FormMainDebugPage();

    QString id() const       { return objectName();   }
    QString name() const;
    QString category() const;

    void refreshContents();

    QWidget *widget();

private:
    QPointer<QWidget> m_Widget;
    QTreeWidget *tree;
    FormMain *m_Form;
};

} // end Form

#endif // IFORMITEM_H
