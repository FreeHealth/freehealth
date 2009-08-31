/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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

#include <coreplugin/core_exporter.h>
#include <coreplugin/idebugpage.h>
#include <coreplugin/constants.h>

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
 * \version 0.0.8
 * \date 27 Aug 2009
*/

namespace Core {
class IFormWidget;

class FormItemIdentifiants : public QObject
{
public:
    FormItemIdentifiants(QObject *parent =0) : QObject(parent) {}
    ~FormItemIdentifiants() {}

    void setUuid(const QString &uuid);
    int uuid() const;

    // define operator< or > or == for the sorting method of items
private:
    QString m_Name;
    QString m_Uuid;
    int id;
};

namespace Internal{
class FormItemSpecPrivate;
}

class CORE_EXPORT FormItemSpec
{
public:
    enum {
        Spec_Author = 0,
        Spec_License,
        Spec_Category,
        Spec_CreationDate,
        Spec_LastModified,
        Spec_Version,
        Spec_Description,
        Spec_Bibliography,
        Spec_Label,
        Spec_Plugin,
        Spec_IconFileName
    };
    FormItemSpec();
    ~FormItemSpec();

    void setValue(int type, const QVariant &val, const QString &lang = QString(Constants::ALL_LANGUAGE));
    QVariant value(const int type, const QString &lang = QString(Constants::ALL_LANGUAGE)) const;

    QString author(const QString &lang = QString(Constants::ALL_LANGUAGE)) const { return value(Spec_Author,lang).toString(); }
    QString license(const QString &lang = QString(Constants::ALL_LANGUAGE)) const {return value(Spec_License,lang).toString();}
    QString category(const QString &lang = QString(Constants::ALL_LANGUAGE)) const { return value(Spec_Category,lang).toString();}
    QDateTime creationDate(const QString &lang = QString(Constants::ALL_LANGUAGE)) const {return value(Spec_CreationDate,lang).toDateTime();}
    QDateTime modificationDate(const QString &lang = QString(Constants::ALL_LANGUAGE)) const {return value(Spec_LastModified,lang).toDateTime();}
    QString version(const QString &lang = QString(Constants::ALL_LANGUAGE)) const {return value(Spec_Version,lang).toString();}
    QString description(const QString &lang = QString(Constants::ALL_LANGUAGE)) const {return value(Spec_Description,lang).toString();}
    QString bibliography(const QString &lang = QString(Constants::ALL_LANGUAGE)) const {return value(Spec_Bibliography,lang).toString();}
    QString label(const QString &lang = QString(Constants::ALL_LANGUAGE)) const {return value(Spec_Label,lang).toString();}
    QString pluginName(const QString &lang = QString(Constants::ALL_LANGUAGE)) const {return value(Spec_Plugin,lang).toString();}

    void toTreeWidget(QTreeWidgetItem *tree) const;

private:
    Core::Internal::FormItemSpecPrivate *d;
};

namespace Internal{
class FormItemScriptsPrivate;
}

class CORE_EXPORT FormItemScripts
{
public:
    enum {
        Script_OnLoad = 0,
        Script_PostLoad,
        Script_OnDemand,
        Script_OnValueChanged,
        Script_OnValueRequiered,
        Script_OnDependentValueChanged
    };


    FormItemScripts(const QString &lang = Constants::ALL_LANGUAGE,
                    const QString &onLoad = QString::null,
                    const QString &postLoad = QString::null,
                    const QString &onDemand = QString::null,
                    const QString &onValChanged = QString::null,
                    const QString &onValRequiered = QString::null,
                    const QString &onDependentValuesChanged = QString::null
                    );
    ~FormItemScripts();

    void setScript(const int type, const QString &script, const QString &lang = Constants::ALL_LANGUAGE);
    QString script(const int type, const QString &lang = Constants::ALL_LANGUAGE) const;

    void warn() const;

    QString onLoadScript() const {return script(Script_OnLoad);}
    QString postLoadScript() const {return script(Script_PostLoad);}
    QString onDemandScript() const {return script(Script_OnDemand);}
    QString onValueChangedScript() const {return script(Script_OnValueChanged);}
    QString onValueRequieredScript() const {return script(Script_OnValueRequiered);}
    QString onDependentValueChangedScript() const {return script(Script_OnDependentValueChanged);}

    void toTreeWidget(QTreeWidgetItem *tree);
private:
    Internal::FormItemScriptsPrivate *d;
};

namespace Internal {
class FormItemValuesPrivate;
}

class CORE_EXPORT FormItemValues : public QObject
{
    Q_OBJECT
public:
    enum {
        Value_Possible = 0,
        Value_Script,
        Value_Numerical,
        Value_Default,
        Value_Dependency
    };
    FormItemValues(QObject *parent);
    ~FormItemValues();

    void setValue(int type, const int id, const QVariant &val, const QString &language = Core::Constants::ALL_LANGUAGE);
    void setDefaultValue(const QVariant &val, const QString &lang = Core::Constants::ALL_LANGUAGE);

    // this value represents the one the user's define : text of the LineEdit, value of the spinBox...
//    void setSelectedValue(const QVariant &val);
//    QVariant selectedValue() const;

    bool isOptionnal() const;
    void setOptionnal(bool state);

    QStringList values(const int typeOfValues) const;

    // unique or historic-values possibilities
    // actual selected value(s)

    // filename
    void setFileName(const QString &fileName) {m_FileName=fileName;}
    QString fileName() const {return m_FileName;}

    void toTreeWidget(QTreeWidgetItem *tree) const;

private:
    Internal::FormItemValuesPrivate *d;
    QString m_FileName;
};


//class FormItemRights
//{
//public:
//    FormItemRights() {}
//    ~FormItemRights() {}
//private:
//};

class FormItemWidgets : public QObject
{
    Q_OBJECT
public:
    FormItemWidgets(QObject *parent = 0): QObject(parent) {}
    ~FormItemWidgets() {}

    virtual void setFormWidget(QWidget *w) {m_FormWidget=w;}
    virtual QWidget *formWidget() {return m_FormWidget;}

    // for later use
    virtual void setHistoricWidget(QWidget *w) {m_HistoricWidget=w;}
    virtual QWidget *historicWidget() {return m_HistoricWidget;}

private:
    QWidget *m_FormWidget, *m_HistoricWidget;
};


class FormPage;

/**
IFormItem : bas niveau dans la définition de l'item
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

class CORE_EXPORT FormItem : public FormItemIdentifiants
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)

public:
    FormItem(QObject *parent=0) :
            FormItemIdentifiants(parent),
            m_Spec(new FormItemSpec),
            m_Scripts(new FormItemScripts),
            m_Values(new FormItemValues(this))
            {}

    virtual ~FormItem()
    {
        if (m_Spec) delete m_Spec;
        m_Spec = 0;
        if (m_Scripts) delete m_Scripts;
        m_Scripts = 0;
    }

    FormItemSpec *spec() const {return m_Spec;}
    FormItemScripts *scripts() const {return m_Scripts;}
    FormItemValues *valueReferences() const {return m_Values;}

    virtual void setFormWidget(Core::IFormWidget *w) {m_FormWidget=w;}
    virtual IFormWidget *formWidget() {return m_FormWidget;}

    virtual FormItem *createChildItem(const QString &uuid = QString::null);
    virtual FormPage *createPage(const QString &uuid = QString::null) {return 0;}
    virtual QList<FormItem*> formItemChildren() const;

    virtual void addExtraData(const QString &id, const QString &data);
    virtual QHash<QString,QString> extraDatas() const {return m_ExtraDatas;}
    virtual void clearExtraDatas() {m_ExtraDatas.clear();}

    virtual void setValue(const QVariant &value) {}
    virtual QVariant value() const { return QVariant(); }

public Q_SLOTS:
    virtual void languageChanged();

private:
    FormItemSpec *m_Spec;
    FormItemScripts *m_Scripts;
    FormItemValues *m_Values;
    IFormWidget *m_FormWidget;
    QHash<QString, QString> m_ExtraDatas;
};
inline QList<Core::FormItem*> Core::FormItem::formItemChildren() const
{
     QList<Core::FormItem*> list;
     foreach(QObject *o, children()) {
          Core::FormItem *i = qobject_cast<Core::FormItem*>(o);
          if (i) {
              list.append(i);
          }
     }
     return list;
}


class CORE_EXPORT FormPage : public FormItem
{
    Q_OBJECT
public:
    FormPage(QObject *parent=0) :
            FormItem(parent) {}
    ~FormPage() {}

    virtual FormPage *createPage(const QString &uuid = QString::null) {return 0;}

    virtual void languageChanged();

private:
};

class FormMainDebugPage;
class CORE_EXPORT FormMain : public FormItem
{
    Q_OBJECT
public:
    FormMain(QObject *parent=0) :
            FormItem(parent), m_DebugPage(0) {}
    ~FormMain();

    FormPage *createPage(const QString &uuid = QString::null);

    virtual void languageChanged();

    virtual FormMain *formParent() { return qobject_cast<FormMain*>(parent()); }

    void createDebugPage();
    void toTreeWidget(QTreeWidget *tree);

private:
    FormMainDebugPage *m_DebugPage;
};


class CORE_EXPORT FormMainDebugPage : public IDebugPage
{
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

} // end Core

#endif // IFORMITEM_H
