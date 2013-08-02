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
 ***************************************************************************/
#ifndef ICDCODESELECTOR_H
#define ICDCODESELECTOR_H

#include <icdplugin/icd_exporter.h>
#include <icdplugin/icdcontextualwidget.h>

QT_BEGIN_NAMESPACE
class QToolButton;
class QModelIndex;
QT_END_NAMESPACE


namespace ICD {
class IcdSearchModel;

namespace Ui {
    class IcdCodeSelector;
}

class ICD_EXPORT IcdCodeSelector : public IcdContextualWidget
{
    Q_OBJECT

public:
    explicit IcdCodeSelector(QWidget *parent = 0);
    ~IcdCodeSelector();

    void initialize();
    void setModel(IcdSearchModel *model);
    IcdSearchModel *model() const;

Q_SIGNALS:
    void entered(const QVariant &SID);
    void activated(const QVariant &SID);

protected:
    void changeEvent(QEvent *e);

private:
    void populateToolButtons();

private Q_SLOTS:
    void onEntered(const QModelIndex &);
    void onActivated(const QModelIndex &);
    void setSearchByLabel();
    void setSearchByCode();
    void setFilter(const QString &search);

private:
    Ui::IcdCodeSelector *ui;
    QToolButton *m_SearchToolButton;
};

}  // End namespace ICD

#endif // ICDCODESELECTOR_H
