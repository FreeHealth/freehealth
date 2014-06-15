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
#ifndef ICDWIDGETFACTORY_H
#define ICDWIDGETFACTORY_H

#include <icdplugin/icd_exporter.h>
#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

namespace ICD {
class IcdCentralWidget;

class ICD_EXPORT IcdWidgetFactory : public Form::IFormWidgetFactory
{
public:
    Q_OBJECT
public:
    IcdWidgetFactory(QObject *parent = 0);
    ~IcdWidgetFactory();

    bool initialize(const QStringList &arguments, QString *errorString);
    bool extensionInitialized();
    bool isInitialized() const;

    bool isContainer(const int) const {return false;}
    QStringList providedWidgets() const;
    Form::IFormWidget *createWidget(const QString &name, Form::FormItem *object, QWidget *parent = 0);
};

class IcdFormData;

class IcdFormWidget : public Form::IFormWidget
{
    Q_OBJECT
    friend class ICD::IcdFormData;
public:
    IcdFormWidget(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~IcdFormWidget();

    void addWidgetToContainer(Form::IFormWidget *) {}
    bool isContainer() const {return false;}

    QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
    void retranslate();

protected:
    IcdCentralWidget *m_CentralWidget;

};

// Used to pass episode date, label, user...
class IcdFormData : public Form::IFormItemData
{
public:
    IcdFormData(Form::FormItem *item);
    ~IcdFormData();

    void setForm(IcdFormWidget *form) {m_Form = form; clear();}
    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;
    void setModified(bool modified);

    void setReadOnly(bool readOnly);
    bool isReadOnly() const;

    // Use setData/Data for episode data
    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    // Storable data
    void setStorableData(const QVariant &data);
    QVariant storableData() const;

private:
    Form::FormItem *m_FormItem;
    IcdFormWidget *m_Form;
    QString m_OriginalValue;
};

}  // End namespace ICD


#endif // ICDWIDGETFACTORY_H
