/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_LANGUAGECOMBOBOXDELEGATE_H
#define UTILS_LANGUAGECOMBOBOXDELEGATE_H

#include <utils/global_exporter.h>
#include <QItemDelegate>

namespace Utils {
namespace Internal {
class LangageComboBoxDelegatePrivate;
}

class UTILS_EXPORT LanguageComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    enum DisplayMode {
        AllLanguages = 0,
        AvailableTranslations
    };

    explicit LanguageComboBoxDelegate(QObject *parent = 0, DisplayMode mode = AvailableTranslations);
    ~LanguageComboBoxDelegate();

    void setTranslationsPath(const QString &absFullPath);
    void setFlagsIconPath(const QString &absFullPath);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    Internal::LangageComboBoxDelegatePrivate *d;
};

}  // End namespace Utils


#endif // UTILS_LANGUAGECOMBOBOXDELEGATE_H
