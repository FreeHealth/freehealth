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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/***************************************************************************
 *   Code adapted from the Qxt Librairy (LGPL 2.1)                         *
 ***************************************************************************/
#ifndef LANGUAGECOMBOBOX_H
#define LANGUAGECOMBOBOX_H

#include <QComboBox>
#include <QLocale>
#include <listviewplugin/listview_exporter.h>

namespace Views {
namespace Internal {
class LanguageComboBoxPrivate;
}  // End namespace Internal

class LISTVIEW_EXPORT LanguageComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit LanguageComboBox(QWidget* parent = 0);
    virtual ~LanguageComboBox();

    enum DisplayMode {
        AllLanguages,
        AvailableTranslations
    };

    DisplayMode displayMode() const;
    QLocale::Language currentLanguage() const;
    QString currentLanguageName() const;

public Q_SLOTS:
    void setCurrentLanguage(const QString &languageIsoCode);
    void setCurrentLanguage(QLocale::Language lang);
    void setDisplayMode(DisplayMode mode);

private Q_SLOTS:
    void comboBoxCurrentIndexChanged(int id);

Q_SIGNALS:
    void currentLanguageChanged(QLocale::Language country);
    void currentLanguageNameChanged(const QString &name);

private:
    Internal::LanguageComboBoxPrivate *d;
};


}  // End namespace Views

#endif // LANGUAGECOMBOBOX_H
