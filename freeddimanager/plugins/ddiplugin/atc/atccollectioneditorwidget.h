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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEDDIMANAGER_ATCCOLLECTIONEDITORWIDGET_H
#define FREEDDIMANAGER_ATCCOLLECTIONEDITORWIDGET_H

#include <ddiplugin/ddi_exporter.h>
#include <QSortFilterProxyModel>

#include <QWidget>
QT_BEGIN_NAMESPACE
class QLineEdit;
class QModelIndex;
class QToolButton;
class QAction;
QT_END_NAMESPACE

/**
 * \file atccollectioneditorwidget.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 09 Oct 2013
*/

namespace Utils {
class DetailsWidget;
}

namespace DDI {
namespace Internal {
class AtcCollectionEditorWidgetPrivate;
} // Internal

class DDI_EXPORT AtcCollectionEditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AtcCollectionEditorWidget(QWidget *parent = 0);
    ~AtcCollectionEditorWidget();

private Q_SLOTS:
    void onAtcCodeSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
    void onFilterChanged(const QString &filter);
    void enabledDataMapperEnabled();
    void submitDataMapper();
    void restoreDataMapper();
    void onNewItemRequested();

private:
    void retranslateUi();
    void changeEvent(QEvent *e);

#ifdef WITH_TESTS
public Q_SLOTS:
    void test_runAllTests();
    void test_views();
    void test_editorEnabledState(bool state);
    void test_actions();
    void test_itemCreation();
    void test_edition();
#endif // WITH_TESTS

private:
    Internal::AtcCollectionEditorWidgetPrivate *d;
};

}  //  End namespace DDI

#endif // FREEDDIMANAGER_ATCCOLLECTIONEDITORWIDGET_H
