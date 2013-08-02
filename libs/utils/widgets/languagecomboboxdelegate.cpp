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
#include "languagecomboboxdelegate.h"
#include "languagecombobox.h"

#include <QDebug>

using namespace Utils;
using namespace Internal;

namespace Utils {
namespace Internal {
class LangageComboBoxDelegatePrivate
{
public:
    LangageComboBoxDelegatePrivate() :
        m_DisplayMode(-1)
    {}

    ~LangageComboBoxDelegatePrivate() {}

public:
    int m_DisplayMode;
    QString m_TrPath, m_IconPath;
};
}
}

LanguageComboBoxDelegate::LanguageComboBoxDelegate(QObject *parent, DisplayMode mode) :
        QItemDelegate(parent),
        d(new Internal::LangageComboBoxDelegatePrivate)
{
    d->m_DisplayMode = mode;
}

LanguageComboBoxDelegate::~LanguageComboBoxDelegate()
{
    if (d)
        delete d;
    d = 0;
}

void LanguageComboBoxDelegate::setTranslationsPath(const QString &absFullPath)
{
    d->m_TrPath = absFullPath;
}

void LanguageComboBoxDelegate::setFlagsIconPath(const QString &absFullPath)
{
    d->m_IconPath = absFullPath;
}

QWidget *LanguageComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const
{
    Q_UNUSED(option);
    LanguageComboBox *editor = new LanguageComboBox(parent);
    editor->setTranslationsPath(d->m_TrPath);
    editor->setFlagsIconPath(d->m_IconPath);
    if (d->m_DisplayMode == AllLanguages)
        editor->setDisplayMode(LanguageComboBox::AllLanguages);
    else
        editor->setDisplayMode(LanguageComboBox::AvailableTranslations);
    editor->setCurrentLanguage(static_cast<QLocale::Language>(index.data(Qt::EditRole).toInt()));
    return editor;
}

void LanguageComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    LanguageComboBox *combo = qobject_cast<LanguageComboBox*>(editor);
    if (combo) {
        combo->setCurrentLanguage(static_cast<QLocale::Language>(index.data(Qt::EditRole).toInt()));
    }
}

void LanguageComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                  const QModelIndex &index) const
{
    LanguageComboBox *combo = qobject_cast<LanguageComboBox*>(editor);
    if (combo) {
        model->setData(index, combo->currentLanguage(), Qt::EditRole);
    }
}

void LanguageComboBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

