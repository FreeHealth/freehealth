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
 ***************************************************************************/
#ifndef GIRWIDGET_H
#define GIRWIDGET_H

#include <aggirplugin/aggir_exporter.h>

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

#include <QWidget>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QAbstractButton;
QT_END_NAMESPACE

/**
 * \file girwidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 15 Sept 2011
*/

namespace Gir {
class GirModel;

namespace Internal {

class GirWidgetFactory : public Form::IFormWidgetFactory
{
    Q_OBJECT
public:
    GirWidgetFactory(QObject *parent = 0);
    ~GirWidgetFactory() {}

    bool initialize(const QStringList &, QString *) {return true;}
    bool extensionInitialized() {return true;}
    bool isInitialized() const {return true;}

    bool isContainer(const int idInStringList) const {Q_UNUSED(idInStringList); return false;}
    QStringList providedWidgets() const {return QStringList() << "aggir" << "gir";}
    Form::IFormWidget *createWidget(const QString &name, Form::FormItem *object, QWidget *parent = 0);
};

//--------------------------------------------------------------------------------------------------------
//--------------------------------------- GirWidget implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
namespace Ui {
class GirWidget;
class GirItem;
}

class GirUi : public QWidget
{
    Q_OBJECT
public:
    GirUi(QWidget *parent = 0);
    ~GirUi();

    void setStringfiedGirScore(const QString &gir);
    QString stringfiedGirScore() const;
    void clearModel();

    void setHtmlPrintMask(const QString &html) {_html=html;}
    QString toHtml() const;

private Q_SLOTS:
    void girCalculated(const int gir);

private:
    Ui::GirWidget *m_ui;
    QString m_GirString, _html;
    int m_Gir;
    GirModel *model;
};

class GirWidget : public Form::IFormWidget
{
    Q_OBJECT
public:
    GirWidget(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~GirWidget();

    bool isContainer() const {return false;}

    void setValue(const QVariant &);
    QVariant value() const;

    void setStringfiedGirScore(const QString &gir);
    QString stringfiedGirScore() const;
    void clearModel();

    QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
    void retranslate() {}

public:
    GirUi *m_ui;
};

class GirItemData : public Form::IFormItemData
{
public:
    explicit GirItemData(Form::FormItem *parent);
    virtual ~GirItemData() {}
    virtual Form::FormItem *parentItem() const {return m_Parent;}

    void clear();
    bool isModified() const;
    void setModified(bool modified);

    void setReadOnly(bool readOnly);
    bool isReadOnly() const;

    void setGirWidget(GirWidget *widget) {m_GirWidget = widget;}

    virtual bool setData(const int ref, const QVariant &data, const int role);
    virtual QVariant data(const int ref, const int role) const;

    virtual void setStorableData(const QVariant &data);
    virtual QVariant storableData() const;

private:
    Form::FormItem *m_Parent;
    QString m_OriginalValue;
    GirWidget *m_GirWidget;
    bool m_modified, m_readonly;
};

} // End Internal
} // End Gir

#endif // GIRWIDGET_H
