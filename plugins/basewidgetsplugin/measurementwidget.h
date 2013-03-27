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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MEASUREMENTWIDGET_H
#define MEASUREMENTWIDGET_H

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

QT_BEGIN_NAMESPACE
class QComboBox;
class QDoubleSpinBox;
QT_END_NAMESPACE

/**
 * \file measurementwidget.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 10 Feb 2013
*/

namespace BaseWidgets {
class MeasurementWidgetData;

class MeasurementWidget : public Form::IFormWidget
{
    Q_OBJECT
    friend class BaseWidgets::MeasurementWidgetData;

public:
    MeasurementWidget(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~MeasurementWidget();

    void setTabOrder(bool consoleWarn);
    void addWidgetToContainer(Form::IFormWidget *) {}
    bool isContainer() const {return false;}

    QString printableHtml(bool withValues = true) const;

public Q_SLOTS:
    void retranslate();

private:
    void populateWithLength();
    void populateWithWeight();

protected:
    QComboBox *m_units;
    QDoubleSpinBox *m_value;
    int m_defaultUnitId;
};

class MeasurementWidgetData : public Form::IFormItemData
{
    Q_OBJECT
public:
    MeasurementWidgetData(Form::FormItem *item);
    ~MeasurementWidgetData();

    void setMeasurementWidget(MeasurementWidget *widget) {m_Measurement = widget; clear();}
    void setDefaultUnitIndex(int index) {m_defaultUnitId = index;}
    void setSelectedUnit(const QString &uuid);
    QString selectedUnitUuid() const;

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
    MeasurementWidget *m_Measurement;
    int m_defaultUnitId;
    QString m_OriginalValue;
};

}  // End namespace BaseWidgets

#endif // MEASUREMENTWIDGET_H
