/***************************************************************************
 *  Original work:                                                         *
 *  The FreeMedForms project www.freemedforms.com                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *                                                                         *
 *  Modified work:                                                         *
 *  FreeHealth EHR                                                         *
 *  Copyright 2016 Jerome Pinguet                                          *
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
 *  Authors:                                                               *
 *  Eric MAEKER, <eric.maeker@gmail.com>                                   *
 *  Jerome Pinguet, <jerome@jerome.cc                                      *
 *  ...                                                                    *
 ***************************************************************************/
#pragma once

#include <utils/widgets/webpage.h>

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

#include <QStringList>

/**
 * \file webpagefactory.h
 * \author Jerome Pinguet <jerome@jerome.cc>
 * \version 1
 * \date 20161211
*/

class QWebEngineView;

namespace BaseWidgets {

class WebPageFactory : public Form::IFormWidgetFactory
{
    Q_OBJECT
public:
    WebPageFactory(QObject *parent = 0);
    ~WebPageFactory();

    bool initialize(const QStringList &arguments, QString *errorString);
    bool extensionInitialized();
    bool isInitialized() const;

    bool isContainer(const int idInStringList) const;
    QStringList providedWidgets() const;
    Form::IFormWidget *createWidget(const QString &name, Form::FormItem *object, QWidget *parent = 0);
};

class WebPageForm : public Form::IFormWidget
{
    Q_OBJECT
public:
    WebPageForm(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~WebPageForm();

    void addWidgetToContainer(Form::IFormWidget *) {}
    bool isContainer() const {return false;}

    QString printableHtml(bool withValues = true) const;

signals:
//    void html(QString sHtml);

public Q_SLOTS:
    void retranslate();

protected slots:
//    void handleHtml(QString sHtml);

private:
    WebPage *m_Web;
    QWebEngineView *m_WebEngineView;
};

class WebPageData : public Form::IFormItemData
{
    Q_OBJECT
public:
    WebPageData(Form::FormItem *item);
    ~WebPageData();

    void setWebPage(WebPage* web) {m_Page = web; clear();}

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

public Q_SLOTS:
    void onValueChanged();

private:
    Form::FormItem *m_FormItem;
    WebPage* m_Page;
    QString m_OriginalValue;
    bool m_ForceModified;
};

}  // End namespace BaseWidgets
