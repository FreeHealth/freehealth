/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef BASEWIDGETS_INTERNAL_FRENCHSOCIALNUMBERWIDGET_H
#define BASEWIDGETS_INTERNAL_FRENCHSOCIALNUMBERWIDGET_H

#include <QWidget>

/**
 * \file frenchsocialnumberwidget.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 2012-12-31
*/

namespace BaseWidgets {
namespace Internal {
class FrenchSocialNumberWidgetPrivate;

class FrenchSocialNumberWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FrenchSocialNumberWidget(QWidget *parent = 0);
    ~FrenchSocialNumberWidget();
    bool initialize();
    void clear();

    void setNumberWithControlKey(const QString &number);
    void setNumberWithoutControlKey(const QString &number);

    bool isValid() const;
    bool isValid(const QString &number, const QString &key) const;
    int controlKey(const QString &number) const;

    QString numberWithControlKey() const;
    QString numberWithoutControlKey() const;

    QString emptyHtmlMask() const;
    QString toHtml() const;

private Q_SLOTS:
    void checkControlKey();

private:
    Internal::FrenchSocialNumberWidgetPrivate *d;
};

} // namespace Internal
} // namespace BaseWidgets

#endif // BASEWIDGETS_INTERNAL_FRENCHSOCIALNUMBERWIDGET_H

