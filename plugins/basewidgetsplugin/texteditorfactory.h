/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TEXTEDITORFACTORY_H
#define TEXTEDITORFACTORY_H

#include <texteditorplugin/texteditor.h>

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

#include <QStringList>

/**
 * \file texteditorfactory.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 15 Sept 2011
*/

namespace BaseWidgets {

class TextEditorFactory : public Form::IFormWidgetFactory
{
    Q_OBJECT
public:
    TextEditorFactory(QObject *parent = 0);
    ~TextEditorFactory();

    bool initialize(const QStringList &arguments, QString *errorString);
    bool extensionInitialized();
    bool isInitialized() const;

    bool isContainer(const int idInStringList) const;
    QStringList providedWidgets() const;
    Form::IFormWidget *createWidget(const QString &name, Form::FormItem *object, QWidget *parent = 0);
};

class TextEditorForm : public Form::IFormWidget
{
    Q_OBJECT
public:
    TextEditorForm(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~TextEditorForm();

    void addWidgetToContainer(Form::IFormWidget *) {}
    bool isContainer() const {return false;}

    QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
    void retranslate();

private:
    Editor::TextEditor *m_Text;
};

class TextEditorData : public Form::IFormItemData
{
    Q_OBJECT
public:
    TextEditorData(Form::FormItem *item);
    ~TextEditorData();

    void setEditor(Editor::TextEditor* editor) {m_Editor = editor; clear();}

    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;
    void setModified(bool modified);

    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    void setStorableData(const QVariant &data);
    QVariant storableData() const;

public Q_SLOTS:
    void onValueChanged();

private:
    Form::FormItem *m_FormItem;
    Editor::TextEditor* m_Editor;
    QString m_OriginalValue;
    bool m_ForceModified;
};

}  // End namespace BaseWidgets

#endif // TEXTEDITORFACTORY_H
