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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACKPREFERENCE_H
#define DATAPACKPREFERENCE_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>
#include <QWidget>

/**
 * \file datapackpreference.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;
}

namespace DataPack {
class Pack;
}

namespace DataPackPlugin {
namespace Internal {
namespace Ui {
    class DataPackPreference;
}

class DataPackPreferenceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataPackPreferenceWidget(QWidget *parent = 0);
    ~DataPackPreferenceWidget();

    void setDataToUi();
    static void writeDefaultSettings(Core::ISettings *s = 0);

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void packInstalled(const DataPack::Pack &pack);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DataPackPreference *ui;
};

class DataPackPreferencePage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    DataPackPreferencePage(QObject *parent = 0);
    ~DataPackPreferencePage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void apply();
    void finish();

    QString helpPage();

    static void writeDefaultSettings(Core::ISettings *s) {DataPackPreferenceWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<DataPackPreferenceWidget> m_Widget;
};

}  // End namespace Internal
}  // End namespace DataPackPlugin

#endif // DATAPACKPREFERENCE_H
