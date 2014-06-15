/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef COMBOWITHFANCYBUTTON_H
#define COMBOWITHFANCYBUTTON_H

#include <utils/global_exporter.h>
#include <QComboBox>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QTreeView;
class QSettings;
class QString;
class QStringListModel;
QT_END_NAMESPACE

#include <QDebug>

/**
 * \file combowithfancybutton.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 25 Feb 2013
*/

namespace Utils {
namespace Internal {
class ItemDelegate;
class StringModel;
}

class UTILS_EXPORT ComboWithFancyButton : public QComboBox
{
    Q_OBJECT

public:
    ComboWithFancyButton(QWidget *parent = 0);

    void setRemoveItems(bool state);
    void setMoveItems(bool state);

    void fancyAddItems(const QStringList &list, const QVariant &userData = QVariant());
    void fancyAddItem(const QString &text, const QVariant &userData = QVariant());
    QStringList fancyItems(const QVariant &userData = QVariant()) const;

    void setRemoveLightIcon(const QIcon &icon);
    void setMoveUpLightIcon(const QIcon &icon);
    void setMoveDownLightIcon(const QIcon &icon);

public Q_SLOTS:
    void fancyClear();

protected Q_SLOTS:
    void handlePressed(const QModelIndex &index);

protected:
    // From virtual members of QComboBox
    void showPopup();
    void hidePopup();

private:
    Internal::ItemDelegate *delegate;
    QTreeView *view;
    QSettings *m_Settings;
    Internal::StringModel *stringModel;
    QString m_Key;
    bool m_ignoreHide, m_editableState;
    int m_Index;
    QString m_Text;
};

}  // End namespace Utils

#endif // COMBOWITHFANCYBUTTON_H
