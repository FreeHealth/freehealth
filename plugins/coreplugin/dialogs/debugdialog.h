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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef COREDEBUGDIALOG_H
#define COREDEBUGDIALOG_H

#include <QDialog>
#include <QHash>
#include <QMessageBox>
#include <utils/messagesender.h>

/**
 * \file debugdialog.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.9
 * \date 09 Sept 2009
*/

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
class QStackedLayout;
QT_END_NAMESPACE

namespace Core {
class IDebugPage;
}

namespace Core {
namespace Internal {

namespace Ui {
    class DebugDialog;
}

class DebugDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(DebugDialog)

public:
    explicit DebugDialog(QWidget * parent);
    ~DebugDialog();

private Q_SLOTS:
    bool saveLogToFile();

private:
    Ui::DebugDialog *m_ui;
    QStackedLayout  *m_slayout;
    QHash<QTreeWidgetItem *, QWidget *> m_Widgets;
    Utils::MessageSender  m_sender;
    bool            m_sending;
    QMessageBox *   m_infoMessageBox;
    bool            m_MessageViewExpanded, m_ErrorViewExpanded;
};

} // End Internal
} // End Core

#endif // COREDEBUGDIALOG_H
