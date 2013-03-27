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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/***************************************************************************
 *   Code adapted from the Qxt Librairy (LGPL 2.1)                         *
 ***************************************************************************/
#ifndef UTILS_LANGUAGECOMBOBOX_H
#define UTILS_LANGUAGECOMBOBOX_H

#include <utils/global_exporter.h>

#include <QComboBox>
#include <QLocale>

namespace Utils {
namespace Internal {
class LanguageComboBoxPrivate;
}  // End namespace Internal

class UTILS_EXPORT LanguageComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QLocale::Language currentLanguage READ currentLanguage WRITE setCurrentLanguage NOTIFY currentLanguageChanged)
    Q_PROPERTY(QString currentLanguageIso READ currentLanguageIsoName WRITE setCurrentIsoLanguage NOTIFY currentLanguageIsoChanged)

public:
    explicit LanguageComboBox(QWidget* parent = 0);
    virtual ~LanguageComboBox();

    enum DisplayMode {
        AllLanguages,
        AvailableTranslations
    };

    void setTranslationsPath(const QString &absFullPath);
    void setFlagsIconPath(const QString &absFullPath);

    DisplayMode displayMode() const;
    QLocale::Language currentLanguage() const;
    QString currentLanguageName() const;
    QString currentLanguageIsoName() const;

public Q_SLOTS:
    void setCurrentIsoLanguage(const QString &languageIsoCode);
    void setCurrentLanguage(QLocale::Language lang);
    void setDisplayMode(DisplayMode mode);

protected Q_SLOTS:
    void comboBoxCurrentIndexChanged(int id);

Q_SIGNALS:
    void currentLanguageChanged(QLocale::Language language);
    void currentLanguageNameChanged(const QString &name);
    void currentLanguageIsoChanged(const QString &iso);

private:
    Internal::LanguageComboBoxPrivate *d;
};

}  // End namespace Utils

#endif // UTILS_LANGUAGECOMBOBOX_H
