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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORMMANAGER_PLUGIN_IFORMITEM_H
#define FORMMANAGER_PLUGIN_IFORMITEM_H

#include <coreplugin/idebugpage.h>

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
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 22 Sept 2012
*/

namespace Core {
class IMode;
}

namespace Form {
namespace Internal {
class FormItemPrivate;
}
class IFormWidget;
class IFormItemData;
class FormItemIdentifier;
class FormPage;
class FormMain;
class IFormIO;
class FormMainDebugPage;
class FormPlaceHolder;

class FORM_EXPORT FormItem : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(FormItem)
//    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)

public:
    FormItem(QObject *parent=0);
    virtual ~FormItem();

    Form::FormItem *parentFormItem() const;
    Form::FormMain *parentFormMain() const;

    QString uuid() const;
    void setUuid(const QString &uuid);

    Form::FormItemSpec *spec() const;
    Form::FormItemScripts *scripts() const;
    Form::FormItemValues *valueReferences() const;

    // Access to database values. Pointer will not be deleted
    void setItemData(Form::IFormItemData *data);
    Form::IFormItemData *itemData();

    // Access to the user's widget
    virtual void setFormWidget(Form::IFormWidget *w);
    virtual IFormWidget *formWidget();

    // Access to the FormItem tree
    virtual FormMain *createChildForm(const QString &uuid = QString::null);
    virtual FormItem *createChildItem(const QString &uuid = QString::null);
    virtual FormPage *createPage(const QString &uuid = QString::null);
    virtual QList<FormItem *> formItemChildren() const;
    virtual QList<FormItem *> flattenedFormItemChildren() const;

    // FormIO extra data
    virtual void addExtraData(const QString &key, const QString &value);
    virtual QHash<QString,QString> extraData() const;
    virtual void clearExtraData();
    virtual QStringList getOptions() const;

    // Data representation
    virtual void setPatientDataRepresentation(const int ref);
    virtual int patientDataRepresentation() const;

public Q_SLOTS:
    virtual void languageChanged();

private:
    Internal::FormItemPrivate *d_ifi;
};
/** Returns the first level Form::FormItem children. */
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
/** Returns all level Form::FormItem children. */
inline QList<FormItem *> Form::FormItem::flattenedFormItemChildren() const
{
    QList<Form::FormItem *> list;
    foreach(QObject *object, children()) {
         Form::FormItem *item = qobject_cast<Form::FormItem*>(object);
         if (item) {
             list.append(item);
             list.append(item->flattenedFormItemChildren());
         }
    }
    return list;
}
/** Returns the Form::FormItem parent. */
inline Form::FormItem *Form::FormItem::parentFormItem() const
{
    return qobject_cast<Form::FormItem *>(parent());
}
/** Returns the Form::FormMain parent. */
inline Form::FormMain *Form::FormItem::parentFormMain() const
{
    QObject *o = this->parent();
    while (o) {
        FormMain *f = qobject_cast<Form::FormMain *>(o);
        if (f)
            return f;
        o = o->parent();
    }
    return 0;
}


class FORM_EXPORT FormPage : public FormItem
{
    Q_OBJECT
public:
    FormPage(QObject *parent=0);
    ~FormPage();

    virtual void languageChanged();
    void specLoaded();

private Q_SLOTS:
    void onPatientFormsLoaded();

private:
    Core::IMode *_mode;
    Form::FormPlaceHolder *_placeHolder;
    bool _inPool;
};

class FORM_EXPORT FormMain : public FormItem
{
    Q_OBJECT
public:
    enum EpisodeCountPossibilities {
        NoEpisode = 0,
        UniqueEpisode,
        MultiEpisode
    };

    enum FormHeader {
        NoHeader = 0,
        GenericHeader,
        AdministrativeHeader,
        PrescriptionHeader
    };

    FormMain(QObject *parent = 0);
    ~FormMain();

    void setIoFormReader(IFormIO *reader);
    IFormIO *reader() const;

    bool isEmptyRootForm() const {return (m_Reader!=0);}

    void setModeUniqueName(const QString &modeUid) {m_ModeName = modeUid;}
    QString modeUniqueName() const {return m_ModeName;}

    void setUseNameAsNSForSubItems(const bool b) {m_UseNameAsNSForSubItems=b;}
    bool useNameAsNSForSubItems() const {return m_UseNameAsNSForSubItems;}

    FormMain *createChildForm(const QString &uuid = QString::null);
    FormPage *createPage(const QString &uuid = QString::null);

    virtual void languageChanged();
    void clear();

    virtual FormMain *formParent() const;
    virtual FormMain *rootFormParent() const;
    virtual QList<FormMain *> flattenedFormMainChildren() const;
    virtual QList<FormMain *> firstLevelFormMainChildren() const;
    virtual FormMain *formMainChild(const QString &uuid) const;

    virtual void setEpisodePossibilities(const int i) {m_Episode = i;}
    virtual int episodePossibilities() const {return m_Episode;}
    bool isUniqueEpisode() const {return m_Episode == UniqueEpisode;}
    bool isNoEpisode() const {return m_Episode == NoEpisode;}
    bool isMultiEpisode() const {return m_Episode == MultiEpisode;}

    virtual void setFormPrintHeader(const int i) {m_Header = i;}
    virtual int formPrintHeader() const {return m_Header;}
    bool isNoHeader() const {return m_Header == NoHeader;}
    bool isGenericHeader() const {return m_Header == GenericHeader;}
    bool isAdministrativeHeader() const {return m_Header == AdministrativeHeader;}
    bool isPrescriptionHeader() const {return m_Header == PrescriptionHeader;}

    QString printableHtml(bool withValues = true) const;

    // Specific to FormMain
    void createDebugPage();
    void toTreeWidget(QTreeWidget *tree);

    void emitFormLoaded();

Q_SIGNALS:
    void formLoaded();
//    void itemAdded(const QString &uuid);
//    void subFormAdded(const QString &uuid);

private:
    Form::IFormIO *m_Reader;
    FormMainDebugPage *m_DebugPage;
    int m_Episode;
    int m_Header;
    QString m_ModeName;
    QList<QPixmap> m_Shots;
    bool m_UseNameAsNSForSubItems;
};
inline Form::FormMain *Form::FormMain::formParent() const
{
    return qobject_cast<FormMain*>(parent());
}
/**
 * Returns the Empty Root Form parent of the Form::FormMain.
 * If the parent is not found, the current object is returned
 */
inline Form::FormMain *Form::FormMain::rootFormParent() const
{
    if (isEmptyRootForm())
        return const_cast<Form::FormMain*>(this);
    Form::FormMain *parent = formParent();
    while (parent) {
        if (parent->isEmptyRootForm())
            return parent;
        parent = parent->formParent();
    }
    return const_cast<Form::FormMain*>(this);
}
/** Returns all Form::FormMain children of the Form::FormMain  (all levels). */
inline QList<Form::FormMain *> Form::FormMain::flattenedFormMainChildren() const
{
     QList<Form::FormMain *> list;
     foreach(QObject *o, children()) {
          Form::FormMain *i = qobject_cast<Form::FormMain*>(o);
          if (i) {
              list.append(i);
              list.append(i->flattenedFormMainChildren());
          }
     }
     return list;
}
/** Returns all Form::FormMain first level children of the Form::FormMain. */
inline QList<FormMain *> Form::FormMain::firstLevelFormMainChildren() const
{
    QList<Form::FormMain *> list;
    foreach(QObject *o, children()) {
         Form::FormMain *i = qobject_cast<Form::FormMain*>(o);
         if (i) {
             list.append(i);
         }
    }
    return list;
}
/** Returns the Form::FormMain child with the uuid \e uuid. */
inline Form::FormMain *Form::FormMain::formMainChild(const QString &uuid) const
{
    QList<FormMain *> forms = flattenedFormMainChildren();
    for(int i=0; i<forms.count(); ++i) {
        FormMain *form = forms.at(i);
        if (form->uuid()==uuid)
            return form;
    }
    return 0;
}

class FORM_EXPORT FormMainDebugPage : public Core::IDebugPage
{
    Q_OBJECT
public:
    FormMainDebugPage(FormMain *form, QObject *parent);
    ~FormMainDebugPage();

    QString id() const       { return objectName();   }
    QString displayName() const;
    QString category() const;
    int sortIndex() const;

    void refreshContents();

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<QWidget> m_Widget;
    QTreeWidget *tree;
    FormMain *m_Form;
};

} // end Form

//QDebug operator<<(QDebug dbg, const Form::FormItem &c);
//QDebug operator<<(QDebug dbg, Form::FormItem *c);
//QDebug operator<<(QDebug dbg, const Form::FormMain &c);
//QDebug operator<<(QDebug dbg, Form::FormMain *c);

#endif // FORMMANAGER_PLUGIN_IFORMITEM_H
