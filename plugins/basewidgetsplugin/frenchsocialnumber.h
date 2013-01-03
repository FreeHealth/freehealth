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
 *   Main developers : Eric MAEKER, MD <eric.maeker@gmail.com>              *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FRENCHSOCIALNUMBER_H
#define FRENCHSOCIALNUMBER_H

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

#include <QWidget>
#include <QVariant>
QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

/**
 * \file frenchsocialnumber.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.2
 * \date 30 Dec 2012
*/

namespace BaseWidgets {
namespace Internal {
class FrenchSocialNumberWidget;
}  // End namespace Internal

class FrenchSocialNumberFormData;

class FrenchSocialNumberFormWidget: public Form::IFormWidget
{
    Q_OBJECT
public:
    FrenchSocialNumberFormWidget(Form::FormItem *formItem, QWidget *parent = 0);
    ~FrenchSocialNumberFormWidget();

    QString printableHtml(bool withValues = true) const;

    void addWidgetToContainer(Form::IFormWidget *) {}
    bool isContainer() const {return false;}

public Q_SLOTS:
    void retranslate();

private:
    FrenchSocialNumberFormData *m_ItemData;
    Internal::FrenchSocialNumberWidget *m_NSS;
};

class FrenchSocialNumberFormData : public Form::IFormItemData
{
public:
    FrenchSocialNumberFormData(Form::FormItem *item);
    ~FrenchSocialNumberFormData();

    void setWidget(Internal::FrenchSocialNumberWidget *w) {m_Widget = w; clear();}
    void clear();
    void populateWithPatientData();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;
    void setModified(bool modified);

    // Use setData/Data for episode data
    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    void setStorableData(const QVariant &);
    QVariant storableData() const;

private:
    Form::FormItem *m_FormItem;
    Internal::FrenchSocialNumberWidget *m_Widget;
    QString m_OriginalValue;
};

}  // End namespace BaseWidgets


#endif // FRENCHSOCIALNUMBER_H
