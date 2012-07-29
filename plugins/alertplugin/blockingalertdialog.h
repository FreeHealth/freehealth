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
#ifndef ALERT_BLOCKINGALERTDIALOG_H
#define ALERT_BLOCKINGALERTDIALOG_H

#include <alertplugin/alertplugin_exporter.h>
#include <alertplugin/alertitem.h>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QAbstractButton;
class QToolButton;
QT_END_NAMESPACE

namespace Alert {
namespace Internal {
class BlockingAlertDialogPrivate;
}

namespace Ui {
class BlockingAlertDialog;
class BlockingAlertDialogOverridingComment;
}

class ALERT_EXPORT BlockingAlertResult
{
public:
    BlockingAlertResult() : _override(false), _accepted(false), _remind(false) {}
    ~BlockingAlertResult() {}

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

class ALERT_EXPORT BlockingAlertDialog : public QDialog
{
    Q_OBJECT

    explicit BlockingAlertDialog(const QList<AlertItem> &item,
                                const QString &themedIcon,
                                const QList<QAbstractButton *> &buttons = QList<QAbstractButton *>(),
                                QWidget *parent = 0);

public:
    enum DialogResult {
        NoBlockingAlert = 0,
        BlockingAlertOverridden,
        BlockingAlertAccepted
    };
    ~BlockingAlertDialog();

    bool isOverridingUserCommentRequired() const;
    bool isRemindLaterRequested() const;
    QString overridingComment() const;

    static BlockingAlertResult executeBlockingAlert(const AlertItem &item, const QString &themedIcon = QString::null, QWidget *parent = 0);
    static BlockingAlertResult executeBlockingAlert(const QList<AlertItem> &item, const QString &themedIcon = QString::null, QWidget *parent = 0);
    static BlockingAlertResult executeBlockingAlert(const QList<AlertItem> &item, const QList<QAbstractButton*> &buttons, const QString &themedIcon = QString::null, QWidget *parent = 0);

    static bool applyResultToAlerts(AlertItem &item, const BlockingAlertResult &result);
    static bool applyResultToAlerts(QList<AlertItem> &items, const BlockingAlertResult &result);

private Q_SLOTS:
    void done(int result);
    void remindLater();
    void override();
    void validateUserOverridingComment();

protected:
    void changeEvent(QEvent *e);
    bool eventFilter(QObject *o, QEvent *e);
    void keyPressEvent(QKeyEvent *event);

private:
    Internal::BlockingAlertDialogPrivate *d;
};

} // namespace Alert

#endif // ALERT_BLOCKINGALERTDIALOG_H
