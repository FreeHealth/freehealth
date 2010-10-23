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
#ifndef ICDCODESELECTOR_H
#define ICDCODESELECTOR_H

#include <icdplugin/icdcontextualwidget.h>
QT_BEGIN_NAMESPACE
class QToolButton;
QT_END_NAMESPACE


namespace ICD {
class IcdModel;

namespace Ui {
    class IcdCodeSelector;
}

class IcdCodeSelector : public IcdContextualWidget
{
    Q_OBJECT

public:
    explicit IcdCodeSelector(QWidget *parent = 0);
    ~IcdCodeSelector();

    void initialize();
    void setModel(IcdModel *model);
    IcdModel *model() const;

protected:
    void changeEvent(QEvent *e);

private:
    void populateToolButtons();

private Q_SLOTS:
    void setSearchByLabel();
    void setSearchByCode();
    void setFilter(const QString &search);

private:
    Ui::IcdCodeSelector *ui;
    QToolButton *m_SearchToolButton;
};

}  // End namespace ICD

#endif // ICDCODESELECTOR_H
