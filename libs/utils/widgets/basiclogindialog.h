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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_BASICLOGINDIALOG_H
#define UTILS_BASICLOGINDIALOG_H

#include <utils/global_exporter.h>
#include <QDialog>
QT_BEGIN_NAMESPACE
class QPushButton;
class QTextBrowser;
QT_END_NAMESPACE

/**
 * \file basiclogindialog.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.0
 * \date 24 Oct 2011
*/

namespace Utils {
namespace Ui {
class BasicLoginDialog;
}

class UTILS_EXPORT BasicLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BasicLoginDialog(QWidget *parent = 0);
    ~BasicLoginDialog();

    void setTitle(const QString &title);
    void setToggleViewIcon(const QString &fullAbsPath);
    void setHtmlExtraInformation(const QString &html);
    void focusLogin();
    QString login() const;
    QString password() const;

private Q_SLOTS:
    void onMoreClicked();

private:
    Ui::BasicLoginDialog *ui;
    QPushButton *_more;
    QTextBrowser *_moreBrowser;
};

} // namespace Utils
#endif // UTILS_BASICLOGINDIALOG_H
