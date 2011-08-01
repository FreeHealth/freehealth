/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef CORE_PAGEWIDGET_H
#define CORE_PAGEWIDGET_H

#include <coreplugin/core_exporter.h>

#include <QWidget>
#include <QList>
#include <QLabel>

/**
 * \file pagewidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 12 Jul 2011
*/

namespace Core {
class IGenericPage;

namespace Internal {
namespace Ui{
    class PageWidget;
} // Ui
} // Internal

class CORE_EXPORT PageWidget : public QWidget
{
    Q_OBJECT
public:
    PageWidget(QWidget *parent);
    ~PageWidget();

    template <class T>
    void setPages(const QList<T*> &pages) {
        m_pages.clear();
        for(int i = 0; i < pages.count(); ++i) {
            IGenericPage *p = qobject_cast<IGenericPage*>(pages.at(i));
            if (p)
                m_pages << p;
        }
    }
    void setSettingKey(const QString &settingKey) {m_settingKey = settingKey;}
    void setStartingPage(const QString &initialCategory, const QString &initialPage) {m_currentCategory=initialCategory; m_currentPage=initialPage;}

    void setupUi(bool sortCategoryView = true);

    void expandAllCategories();
    void expandFirstCategories();

    IGenericPage *currentPage() const;
    void saveState();

    QList<QWidget *> pageWidgets() const;

private:
    QWidget *createPageWidget(IGenericPage *page);
    void changeEvent(QEvent *event);

private Q_SLOTS:
    void pageSelected();

private:
    Internal::Ui::PageWidget *m_ui;
    QList<Core::IGenericPage*> m_pages;
    bool m_applied;
    QString m_currentCategory;
    QString m_currentPage;
    QString m_settingKey;
    QList<QWidget *> m_AddedWidgets;
};

} // namespace Core

#endif // CORE_PAGEWIDGET_H
