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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORM_FORMMODE_H
#define FORM_FORMMODE_H

#include <QWidget>
#include <QVariant>

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

/**
 * \file formmode.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 04 Sept 2012
*/

namespace Core {
class BaseMode;
}

namespace Form {
class FormPlaceHolder;

class FormMode : public Form::FormItem
{
    Q_OBJECT
public:
    explicit FormMode(Form::FormItem *item, QWidget *parent = 0);
    ~FormMode();

private Q_SLOTS:
    void getPatientForm();
    void retranslate();

private:
    Form::FormItem *_formItem;
    Core::BaseMode *_mode;
    Form::FormPlaceHolder *_placeHolder;
    bool _inPool;
};

// Used to pass episode date, label, user...
//class ModeData : public Form::IFormItemData
//{
//public:
//    ModeData(Form::FormItem *item);
//    ~ModeData();

//    void setModeWidget(ModeWidget *mode) {m_Mode = mode; clear();}
//    void clear();

//    Form::FormItem *parentItem() const {return m_FormItem;}
//    bool isModified() const;
//    void setModified(bool modified);

//    // Use setData/Data for episode data
//    bool setData(const int ref, const QVariant &data, const int role = Qt::EditRole);
//    QVariant data(const int ref, const int role = Qt::DisplayRole) const;

//    // Used storable data for forms
//    void setStorableData(const QVariant &modified);
//    QVariant storableData() const;

//private:
//    Form::FormItem *m_FormItem;
//    ModeWidget *m_Mode;
//};

} // namespace BaseWidgets

#endif // FORM_FORMMODE_H
