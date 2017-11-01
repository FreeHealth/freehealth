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
#ifndef FREEDDIMANAGER_ATCEDITORWIDGET_H
#define FREEDDIMANAGER_ATCEDITORWIDGET_H

#include <QWidget>
QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file atceditorwidget.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 09 Oct 2013
*/

namespace DDI {
class AtcTableModel;
namespace Internal {
class AtcEditorWidgetPrivate;
}

class AtcEditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AtcEditorWidget(QWidget *parent = 0);
    ~AtcEditorWidget();
    void clear();

    void setAtcTableModel(AtcTableModel *model);
    void setEditorEnabled(bool enabled);
    void setCurrentIndex(const QModelIndex &index);

    bool isEditorEnabled() const;

public Q_SLOTS:
    bool submit();

private Q_SLOTS:
    void populateParents();

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
    Internal::AtcEditorWidgetPrivate *d;
};

}  //  End namespace DDI

#endif // FREEDDIMANAGER_ATCEDITORWIDGET_H
