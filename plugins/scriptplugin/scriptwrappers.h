/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 ***************************************************************************/
#ifndef SCRIPT_SCRIPTWRAPPERS_H
#define SCRIPT_SCRIPTWRAPPERS_H

#include <QObject>
#include <QDate>
#include <QString>
#include <QVariant>
#include <QVector>
#include <QtScript/QScriptable>
#include <QtScript/QScriptValue>
#include <QPointer>
#include <QListWidgetItem>
#include <QListWidget>
#include <QComboBox>

QT_BEGIN_NAMESPACE
class QScriptValue;
class QScriptEngine;
class QScriptContext;
QT_END_NAMESPACE

/**
 * \file scriptwrappers.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.3
 * \date 27 May 2012
*/

namespace Form {
class FormItem;
class FormMain;
}

namespace Script {
namespace Internal {

class ScriptManager;

// FormItem
class FormItemScriptWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool     isValid         READ isValid)
    Q_PROPERTY(QString  uuid            READ uuid)
    Q_PROPERTY(QString  type            READ type)
    Q_PROPERTY(QVariant currentValue    READ currentValue)
    Q_PROPERTY(QVariant currentText     READ currentText        WRITE setCurrentText SCRIPTABLE true)
    Q_PROPERTY(bool     enabled         READ isEnabled          WRITE setEnabled     SCRIPTABLE true)
    Q_PROPERTY(bool     checked         READ isChecked          WRITE setChecked     SCRIPTABLE true)
    Q_PROPERTY(QVariant currentUuid     READ currentUuid        WRITE setCurrentUuid SCRIPTABLE true)

public:
    FormItemScriptWrapper(QObject *parent = 0);
    ~FormItemScriptWrapper();

    void setFormItem(Form::FormItem *item);
    Form::FormItem *item() const;

public Q_SLOTS:
    bool isValid() const;
    QString uuid() const;
    QString type() const;

    QVariant currentValue() const;

    QVariant currentText() const;
    void setCurrentText(const QVariant &text) const;

    bool isChecked() const;
    void setChecked(const bool check);

    QVariant currentUuid() const;
    void setCurrentUuid(const QVariant &uuid);

    void setEnabled(const bool enable);
    bool isEnabled() const;

    QWidget *ui() const;

    QStringList childrenUuid() const;

private:
    QPointer<Form::FormItem> m_Item;
};

class FormMainScriptWrapper: public QObject
{
    Q_OBJECT
public:
    FormMainScriptWrapper(Form::FormMain *formMain);
};

// FormManager
class FormManagerScriptWrapper : public QObject
{
    friend class Script::Internal::ScriptManager;
    Q_OBJECT
    Q_PROPERTY(QString  currentLanguage READ currentLanguage)
    Q_PROPERTY(bool     areLoaded       READ areLoaded)
    Q_PROPERTY(bool     logItemSearch   READ logItemSearch WRITE setLogItemSearch)
    Q_PROPERTY(QString  namespaceInUse  READ namespaceInUse WRITE usingNamespace RESET clearNamespace)

public:
    FormManagerScriptWrapper(QObject *parent = 0);

protected:
    void recreateItemWrappers();
    void updateSubFormItemWrappers(const QString &uuid);

public Q_SLOTS:
    QString currentLanguage() const;
    bool areLoaded() const;
    bool logItemSearch() const {return m_LogItemSearch;}
    void setLogItemSearch(const bool state) {m_LogItemSearch=state;}

    void usingNamespace(const QString &ns) const;
    void endNamespace() const {m_NS.clear();}
    void clearNamespace() const {m_NS.clear();}
    QString namespaceInUse() const {return m_NS;}

    QScriptValue item(const QString &uuid);
    QString extractFormFile(const QString &uuid, const QString &fileName);

//    FormMainScriptWrapper form(const QString &uuid) const;

Q_SIGNALS:
    void languageChanged();

private:
    mutable QString m_NS;
    QHash<QString, QScriptValue> m_Items;
    FormItemScriptWrapper m_Null;
    bool m_LogItemSearch;
    QVector<FormItemScriptWrapper*> m_Wrappers;
};

}  // namespace Internal
}  // namespace Script

Q_DECLARE_METATYPE(Script::Internal::FormItemScriptWrapper*)

#endif // SCRIPT_SCRIPTWRAPPERS_H
