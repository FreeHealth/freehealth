/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#ifndef MFDRUGINFO_P_H
#define MFDRUGINFO_P_H

#include <utils/messagesender.h>

// include Qt headers
#include <QDialog>
#include <QObject>

// include Ui
#include "ui_druginfo.h"


/**
 * \file druginfo.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.1
 * \date 25 Oct 2009
*/

namespace DrugsDB {
namespace Internal {
class DrugsInteraction;
}
}


namespace DrugsWidget {
namespace Internal {
class DrugsData;
class DrugInteraction;

class DrugInfoPrivate : public QObject, public Ui::DrugInfo
{
     Q_OBJECT
public:
     DrugInfoPrivate(QDialog *parent);
     ~DrugInfoPrivate() {}

     bool checkSent();

public Q_SLOTS:
     void on_butSendINN_clicked();
     void on_butIAMSend_clicked();
     void on_listWidgetInteractions_itemSelectionChanged();

public:
     QDialog *m_Parent;
     int m_CIS;
     Utils::MessageSender      m_Sender;
     QList<DrugsDB::Internal::DrugsInteraction *>  m_InteractionsList;         // should not be deleted
     bool                      m_INNSent, m_InteractSent;
};

}  // End Internal
}  // End DrugsWidget

#endif  // MFDRUGINFO_P_H

