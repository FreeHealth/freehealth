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
 *  Main Developers:                                                       *
 *       Christian A Reiter <christian.a.reiter@gmail.com>                 *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef WEBCAM_INTERNAL_WEBCAMPREFERENCES_H
#define WEBCAM_INTERNAL_WEBCAMPREFERENCES_H

#include "webcamphotoprovider.h"

#include <coreplugin/ioptionspage.h>
#include <QWidget>
#include <QPointer>
#include <QMap>

/**
 * \file webcampreferences.h
 * \author Christian A Reiter
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;
}

namespace Webcam {
namespace Internal {
namespace Ui {
class WebcamPreferencesWidget;
}

class WebcamPreferencesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WebcamPreferencesWidget(QWidget *parent = 0);
    ~WebcamPreferencesWidget();

    void setDataToUi();
    QString searchKeywords() const;

    static void writeDefaultSettings(Core::ISettings *s);

    void addWebcamProvider(WebcamPhotoProvider *provider);

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

private:
    void retranslateUi();
    void changeEvent(QEvent *e);

private:
    Ui::WebcamPreferencesWidget *ui;
};


class WebcamPreferencesPage : public Core::IOptionsPage
{
    Q_OBJECT

public:
    WebcamPreferencesPage(QObject *parent = 0);
    ~WebcamPreferencesPage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const {return displayName();}
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void apply();
    void finish();

    bool matches(const QString &searchKeyWord) const;

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {WebcamPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);

private:
    Internal::WebcamPreferencesWidget *m_Widget;
    QString m_searchKeywords;
};


} // namespace Internal
} // namespace Webcam
#endif // WEBCAM_INTERNAL_WEBCAMPREFERENCES_H

