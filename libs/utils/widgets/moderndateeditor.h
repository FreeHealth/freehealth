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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Christian A. Reiter, <christian.a.reiter@gmail.com> *
 *  Contributors:                                                          *
 *       Eric MAEKER <eric.maeker@gmail.com>                               *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_MODERNDATEEDITOR_H
#define UTILS_MODERNDATEEDITOR_H

#include <utils/widgets/qbuttonlineedit.h>
#include <utils/global_exporter.h>

#include <QDate>

QT_BEGIN_NAMESPACE
class QAction;
class QToolButton;
class QString;
QT_END_NAMESPACE

/**
 * \file moderndateeditor.h
 * \author Christian A. Reiter, Eric MAEKER
 * \version 0.8.4
 * \date 22 Feb 2013
*/

namespace Utils {
namespace Internal {
class ModernDateEditorPrivate;
}
class DateValidator;

class UTILS_EXPORT ModernDateEditor : public QButtonLineEdit
{
    friend class Utils::Internal::ModernDateEditorPrivate;
    Q_OBJECT
    Q_PROPERTY(QDate date READ date WRITE setDate NOTIFY dateChanged USER true)
//    Q_PROPERTY(bool calendarPopup READ calendarPopup WRITE setCalendarPopup)

public:
    explicit ModernDateEditor(QWidget *parent = 0);
    explicit ModernDateEditor(const QDate & date, QWidget *parent = 0);
    ~ModernDateEditor();

    // View options
    void setClearIcon(const QString &fullAbsPath);
    void setDateIcon(const QString &fullAbsPath);

    void setDefaultEditingDateFormat(const QString &format);

    QDate date() const;

public Q_SLOTS:
    void clear();
    void setDateString(QString dateString);
    void setDate(const QDate &date);

Q_SIGNALS:
    void dateChanged(const QDate &date);
    void dateChanged();

protected Q_SLOTS:
    void updateDisplayText();
    void onLeftButtonActionTriggered(QAction*);

private:
    void init(const QDate& date = QDate(), const QDate& maximumDate = QDate(), const QDate& minimumDate = QDate());
    void updatePlaceHolder();
    void focusOutEvent(QFocusEvent *event);
    void focusInEvent(QFocusEvent *event);
    void changeEvent(QEvent *e);

protected:
    void retranslate();

private:
    Internal::ModernDateEditorPrivate *d_de;
};

} // end Utils

#endif // UTILS_MODERNDATEEDITOR_H
