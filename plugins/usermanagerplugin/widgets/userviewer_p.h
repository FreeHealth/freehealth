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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TKUSERVIEWER_P_H
#define TKUSERVIEWER_P_H

// include Qt headers
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QDataWidgetMapper>

// include uis
#include "ui_userviewer.h"
//#include "ui_tkHeadersFootersViewerPrivate.h"

/**
 \brief Private part.
 \internal
 \ingroup usertoolkit widget_usertoolkit usermanager
*/

namespace Print {
class Printer;
class PrinterPreviewer;
}

namespace UserPlugin {
namespace Internal {

class UserViewerPrivate : public QObject, public Ui::UserViewer
{
    Q_OBJECT    
public:
    explicit UserViewerPrivate( QObject * parent ); // work with usermodel
    virtual ~UserViewerPrivate() {}

    void initialize();

public:
    void checkUserRights();
    void changeUserIndex( const int modelRow );

public Q_SLOTS:
    void onModelReseted();
    void on_but_changePassword_clicked();
    void on_but_viewHistory_clicked();

private:
    void prepareMapper();

public:
    bool m_CanModify, m_CanRead;
    QWidget *m_Parent;
    int m_Row;
    QDataWidgetMapper *m_Mapper;
    Print::PrinterPreviewer *genericPreview, *adminPreview, *prescriptionPreview;
};

}  // End Internal
}  // End UserPlugin

#endif // TKUSERVIEWER_P_H
