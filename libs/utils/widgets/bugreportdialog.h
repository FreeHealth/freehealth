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
 *   Main developers : Christian A Reiter, Eric Maeker                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_BUGREPORTDIALOG_H
#define UTILS_BUGREPORTDIALOG_H

#include <utils/global_exporter.h>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

/**
 * \file bugreportdialog.h
 * \author Christian A Reiter, Eric Maeker
 * \version 0.8.0
 * \date 20 Dec 2012
*/

namespace Utils {
namespace Internal {
namespace Ui {
class BugReportDialog;
}
} // namespace Internal

class UTILS_EXPORT BugReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BugReportDialog(QWidget *parent = 0);
    ~BugReportDialog();

public slots:
    void on_emailEdit_textChanged(const QString &text);
    void sendBugReport();

protected:
    void changeEvent(QEvent *e);

private:
    Internal::Ui::BugReportDialog *ui;
    QPushButton *m_sendReportButton;
};

} // namespace Utils

#endif // UTILS_BUGREPORTDIALOG_H
