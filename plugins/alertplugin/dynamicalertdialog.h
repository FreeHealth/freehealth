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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ALERT_DYNAMICALERTDIALOG_H
#define ALERT_DYNAMICALERTDIALOG_H

#include <alertplugin/alertplugin_exporter.h>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QAbstractButton;
QT_END_NAMESPACE

namespace Alert {
class AlertItem;
namespace Ui {
class DynamicAlertDialog;
}

class ALERT_EXPORT DynamicAlertResult
{
public:
    DynamicAlertResult() {}
    ~DynamicAlertResult() {}

};

class ALERT_EXPORT DynamicAlertDialog : public QDialog
{
    Q_OBJECT

    explicit DynamicAlertDialog(const QList<AlertItem> &item,
                                const QString &themedIcon,
                                const QList<QAbstractButton *> &buttons = QList<QAbstractButton *>(),
                                QWidget *parent = 0);

public:
    enum DialogResult {
        NoDynamicAlert = 0,
        DynamicAlertOverridden,
        DynamicAlertAccepted
    };
    ~DynamicAlertDialog();

    static DynamicAlertResult executeDynamicAlert(const AlertItem &item, const QString &themedIcon = QString::null, QWidget *parent = 0);
    static DynamicAlertResult executeDynamicAlert(const QList<AlertItem> &item, const QString &themedIcon = QString::null, QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DynamicAlertDialog *ui;
};

} // namespace Alert
#endif // ALERT_DYNAMICALERTDIALOG_H
