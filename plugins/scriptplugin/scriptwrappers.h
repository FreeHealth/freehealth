/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef SCRIPT_SCRIPTWRAPPERS_H
#define SCRIPT_SCRIPTWRAPPERS_H

#include <QObject>
#include <QScriptClass>
#include <QDate>
#include <QString>
#include <QVariant>
#include <QVector>

namespace Form {
class FormItem;
class FormMain;
}

namespace Script {

// FormItem

class FormItemScriptWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool     isValid         READ isValid)
    Q_PROPERTY(QString  uuid            READ uuid)
    Q_PROPERTY(QString  type            READ type)
    Q_PROPERTY(QVariant currentValue    READ currentValue)
    Q_PROPERTY(QVariant currentText     READ currentText        WRITE setCurrentText SCRIPTABLE true)
    Q_PROPERTY(bool     enabled         READ isEnabled          WRITE setEnabled    SCRIPTABLE true)
    Q_PROPERTY(bool     checked         READ isChecked          WRITE setChecked    SCRIPTABLE true)
    Q_PROPERTY(QVariant currentUuid     READ currentUuid        WRITE setCurrentUuid SCRIPTABLE true)

public:
    FormItemScriptWrapper(QObject *parent = 0);
    void setFormItem(Form::FormItem *item);

public Q_SLOTS:
    bool isValid() const {return (m_Item!=0);}
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

private:
    Form::FormItem *m_Item;
};

class FormMainScriptWrapper: public QObject
{
    Q_OBJECT
public:
    FormMainScriptWrapper(Form::FormMain *formMain);
};

}

Q_DECLARE_METATYPE(Script::FormItemScriptWrapper*)



namespace Script {

// FormManager
class FormManagerScriptWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool     areLoaded       READ areLoaded)
    Q_PROPERTY(bool     logItemSearch   READ logItemSearch WRITE setLogItemSearch)
    Q_PROPERTY(QString  namespaceInUse  READ namespaceInUse WRITE usingNamespace RESET clearNamespace)

public:
    FormManagerScriptWrapper(QObject *parent = 0);

public Q_SLOTS:
    bool areLoaded() const;
    bool logItemSearch() const {return m_LogItemSearch;}
    void setLogItemSearch(const bool state) {m_LogItemSearch=state;}

    void usingNamespace(const QString &ns) const;
    void endNamespace() const {m_NS.clear();}
    void clearNamespace() const {m_NS.clear();}
    QString namespaceInUse() const {return m_NS;}

    QScriptValue item(const QString &uuid);
//    FormMainScriptWrapper form(const QString &uuid) const;

private Q_SLOTS:
    void updateItemWrappers();

private:
    mutable QString m_NS;
    QVector<QScriptValue> m_Items;
    FormItemScriptWrapper m_Null;
    bool m_LogItemSearch;
};


} // namespace Script


#endif // SCRIPT_SCRIPTWRAPPERS_H
