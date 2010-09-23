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

/**
 * \file combowithfancybutton.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 18 Aug 2010
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
    explicit ComboWithFancyButton(QWidget *parent = 0);

    void setRemoveItems(bool state);
    void setMoveItems(bool state);

    void addItems(const QStringList &list, const QVariant &userData = QVariant());
    void addItem(const QString & text, const QVariant &userData = QVariant());
    QStringList items(const QVariant &userData = QVariant()) const;


    //    void setSettings(QSettings *settings) {m_Settings = settings;}
    //    void setSettingsKey(const QString &key) {m_Key = key;}
    void saveItemsToSettings();

    void setDarkIcon(const QIcon &icon);
    void setRemoveLightIcon(const QIcon &icon);
    void setMoveUpLightIcon(const QIcon &icon);
    void setMoveDownLightIcon(const QIcon &icon);

public Q_SLOTS:
    void fancyClear();

protected Q_SLOTS:
    void handlePressed(const QModelIndex &index);

protected:
    void showPopup();
    void hidePopup();


private:
    Internal::ItemDelegate *delegate;
    QTreeView *view;
    QSettings *m_Settings;
    Internal::StringModel *model;
    QString m_Key;
    bool m_ignoreHide, m_editableState;
};

}  // End namespace Utils

#endif // COMBOWITHFANCYBUTTON_H
