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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CORE_INTERNAL_NETWORKPREFERENCES_H
#define CORE_INTERNAL_NETWORKPREFERENCES_H

#include <QWidget>

#include <coreplugin/core_exporter.h>
#include <coreplugin/ioptionspage.h>

#include <QWidget>

#include <QPointer>
#include <QObject>

/**
 * \file networkpreferences.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;

namespace Internal {
namespace Ui {
class ProxyPreferencesWidget;
}

class ProxyPreferencesWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ProxyPreferencesWidget)

public:
    explicit ProxyPreferencesWidget(QWidget *parent = 0);

    static void writeDefaultSettings(Core::ISettings *s);
    void setDataToUi();

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

private Q_SLOTS:
    void autoDetectProxy();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::ProxyPreferencesWidget *ui;
};

class CORE_EXPORT ProxyPreferencesPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    ProxyPreferencesPage(QObject *parent = 0);
    ~ProxyPreferencesPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return "parametrer.html";}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::ProxyPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::ProxyPreferencesWidget> m_Widget;
};


} // namespace Internal
} // namespace Core


#endif // CORE_INTERNAL_NETWORKPREFERENCES_H
