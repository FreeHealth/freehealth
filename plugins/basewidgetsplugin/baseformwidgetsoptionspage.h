/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef BASEFORMWIDGETSOPTIONSPAGE_H
#define BASEFORMWIDGETSOPTIONSPAGE_H

#include <coreplugin/ioptionspage.h>
#include <QtCore/QObject>
#include <QtGui/QWidget>

#include <QPointer>

namespace BaseWidgets {
namespace Internal {

namespace Ui {
    class BaseFormWidgetsOptionsPage;
}

class BaseFormSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    BaseFormSettingsWidget(QWidget *parent = 0);
    ~BaseFormSettingsWidget();

    void applyChanges();
    void resetToDefaults();

private:
    Ui::BaseFormWidgetsOptionsPage *m_ui;
};

class BaseFormWidgetsOptionsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    BaseFormWidgetsOptionsPage(QObject *parent = 0);
    ~BaseFormWidgetsOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    /** \todo add help page */
    QString helpPage() {return QString();}

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<BaseFormSettingsWidget> m_Widget;
};

} // End Internal
} // End BaseWidgets

#endif // BASEFORMWIDGETSOPTIONSPAGE_H
