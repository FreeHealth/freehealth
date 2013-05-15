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
 *   Main developers : Eric Maeker                                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef BASEWIDGETS_INTERNAL_BUTTONMENUPATHITEMVIEW_H
#define BASEWIDGETS_INTERNAL_BUTTONMENUPATHITEMVIEW_H

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>
#include <QToolButton>
#include <QFileSystemModel>

/**
 * \file buttonmenupathitemview.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 15 May 2013
*/

namespace Utils {
class QMenuItemView;
}

namespace BaseWidgets {
namespace Internal {

class ButtonMenuPathItemView : public Form::IFormWidget
{
    Q_OBJECT
    
public:
    explicit ButtonMenuPathItemView(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~ButtonMenuPathItemView();
    
    void addWidgetToContainer(Form::IFormWidget *widget);
    bool isContainer() const;
    
    // Printing
    QString printableHtml(bool withValues = true) const;
    
public Q_SLOTS:
    void onIndexTriggered(const QModelIndex &index);
    void retranslate();
    
private:
    Utils::QMenuItemView *_menu;
    QToolButton *_button;
    QFileSystemModel *_model;
};

} // namespace Internal
} // namespace BaseWidgets

#endif // BASEWIDGETS_INTERNAL_BUTTONMENUPATHITEMVIEW_H

