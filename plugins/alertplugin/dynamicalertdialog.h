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
#include <alertplugin/alertitem.h>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QAbstractButton;
class QToolButton;
QT_END_NAMESPACE

namespace Alert {

namespace Ui {
class DynamicAlertDialog;
class DynamicAlertDialogOverridingComment;
}

class ALERT_EXPORT DynamicAlertResult
{
public:
    DynamicAlertResult() : _override(false), _accepted(false), _remind(false) {}
    ~DynamicAlertResult() {}

    void setOverriden(bool override) {_override = override;}
    bool isOverridenByUser() const {return _override;}

    void setOverrideUserComment(const QString &comment) {_overrideComment=comment;}
    QString overrideUserComment() const {return _overrideComment;}

    void setAccepted(bool accepted) {_accepted = accepted;}
    bool isAccepted() const {return _accepted;}

    void setRemindLaterRequested(bool remind) {_remind = remind;}
    bool isRemindLaterRequested() const {return _remind;}

    void setReadAlertUid(const QStringList &uids) {_readUids=uids;}
    QStringList readAlertsUid() const {return _readUids;}

    void setAlertValidation(const AlertValidation &validation) {_validation=validation;}
    AlertValidation alertValidation() const {return _validation;}

private:
    bool _override, _accepted, _remind;
    QString _overrideComment;
    QStringList _readUids;
    AlertValidation _validation;
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

    bool isOverridingUserCommentRequired() const {return _overrideCommentRequired;}
    bool isRemindLaterRequested() const {return _remind;}
    QString overridingComment() const;

    static DynamicAlertResult executeDynamicAlert(const AlertItem &item, const QString &themedIcon = QString::null, QWidget *parent = 0);
    static DynamicAlertResult executeDynamicAlert(const QList<AlertItem> &item, const QString &themedIcon = QString::null, QWidget *parent = 0);
    static DynamicAlertResult executeDynamicAlert(const QList<AlertItem> &item, const QList<QAbstractButton*> &buttons, const QString &themedIcon = QString::null, QWidget *parent = 0);

    static bool applyResultToAlerts(AlertItem &item, const DynamicAlertResult &result);
    static bool applyResultToAlerts(QList<AlertItem> &items, const DynamicAlertResult &result);

private Q_SLOTS:
    void remindLater();
    void override();
    void validateUserOverridingComment();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DynamicAlertDialog *ui;
    Ui::DynamicAlertDialogOverridingComment *cui;
    QDialogButtonBox *_box;
    QToolButton *_overrideButton, *_remindLaterButton;
    bool _overrideCommentRequired, _remind;
};

} // namespace Alert
#endif // ALERT_DYNAMICALERTDIALOG_H
