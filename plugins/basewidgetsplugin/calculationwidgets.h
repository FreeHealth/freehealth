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
#ifndef __CALCULATIONWIDGETS_H
#define __CALCULATIONWIDGETS_H

#include <formmanagerplugin/iformwidgetfactory.h>
#include <texteditorplugin/texteditor.h>
#include <QVariant>
#include <QLineEdit>

/**
 * \file calculationwidgets.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 29 Dec 2011
*/


namespace BaseWidgets {
namespace Internal {

class CalculationWidgetsFactory : public Form::IFormWidgetFactory
{
    Q_OBJECT
public:
    CalculationWidgetsFactory(QObject *parent = 0);
    ~CalculationWidgetsFactory();

    bool initialize(const QStringList &arguments, QString *errorString);
    bool extensionInitialized();
    bool isInitialized() const;

    bool isContainer(const int idInStringList) const;
    QStringList providedWidgets() const;
    Form::IFormWidget *createWidget(const QString &name, Form::FormItem *object, QWidget *parent = 0);
};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- SumWidget implementation ----------------------------------------
//--------------------------------------------------------------------------------------------------------
//class SumWidgetData;
class SumWidget : public Form::IFormWidget
{
    Q_OBJECT
public:
    SumWidget(Form::FormItem *formItem, QWidget *parent = 0);
    ~SumWidget();

    QString printableHtml(bool withValues = true) const;

private Q_SLOTS:
    void retranslate();
    void connectFormItems();
    void recalculate(const int modifiedRef);

private:
    QLineEdit *line;
//    SumWidgetData *m_ItemData;
};

class ScriptWidget : public Form::IFormWidget
{
    Q_OBJECT
public:
    ScriptWidget(Form::FormItem *formItem, QWidget *parent = 0);
    ~ScriptWidget();

    QString printableHtml(bool withValues = true) const;

private Q_SLOTS:
    void retranslate();
    void connectFormItems();
    void recalculate(const int modifiedRef);

private:
    QLineEdit *line;
    Editor::TextEditor* m_Editor;
};

}  // End namespace Internal
}  // End namespace BaseWidgets

#endif // __CALCULATIONWIDGETS_H
