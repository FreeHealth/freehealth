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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORMFILESSELECTORWIDGET_H
#define FORMFILESSELECTORWIDGET_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <QWidget>
#include <QAbstractItemView>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE


/**
 * \file formfileselectorwidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 01 Apr 2011
*/

namespace Form {
class FormIODescription;
namespace Internal {
class FormFilesSelectorWidgetPrivate;
}  // End namespace Internal

class FORM_EXPORT FormFilesSelectorWidget : public QWidget
{
    Q_OBJECT

public:
    enum FormType {
        AllForms = 0,
        CompleteForms,
        SubForms
//        Pages
    };
    enum SelectionType {
        Single = QAbstractItemView::SingleSelection,
        Multiple = QAbstractItemView::MultiSelection
    };

    FormFilesSelectorWidget(QWidget *parent = 0, const FormType type = AllForms, const SelectionType selType = Single);
    ~FormFilesSelectorWidget();

    void setFormType(FormType type);
    void setExcludeGenderSpecific(bool excludeGenderSpecific);
    void setExcludeFormByUid(const QStringList &formuids);
    const QStringList &excludedFormByUid() const;
    void setSelectionType(SelectionType type);
    void expandAllItems() const;
    void setIncludeLocalFiles(bool includeLocal);

    QList<Form::FormIODescription *> selectedForms() const;

    void highlighForm(const QString &uuidOrAbsPath);

private Q_SLOTS:
    void onDescriptionSelected(const QModelIndex &index, const QModelIndex &previous);
    void onFilterSelected();
    void showScreenShot();

protected:
    void changeEvent(QEvent *e);

private:
    Internal::FormFilesSelectorWidgetPrivate *d;
};

}  // End namespace Form

#endif // FORMFILESSELECTORWIDGET_H
