/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef  FINDRECEIPTSVALUES_H
#define  FINDRECEIPTSVALUES_H
#include "ui_findValuesGUI.h"

#include <accountplugin/account_exporter.h>
#include <accountplugin/receipts/receiptsmanager.h>
#include <accountplugin/receipts/xmlcategoriesparser.h>

#include <accountbaseplugin/medicalproceduremodel.h>
#include <accountbaseplugin/thesaurusmodel.h>
#include <accountbaseplugin/constants.h>

#include <QtGui>
#include <QtCore>
#include <QSqlDatabase>

using namespace AccountDB;
namespace Ui{
  class findValueDialog;
}
class ACCOUNT_EXPORT findReceiptsValues:public QDialog
{
  Q_OBJECT
public:
    QHash<QString,QString> returnValuesHash();
    findReceiptsValues(QWidget * parent = 0);
    ~findReceiptsValues();
    QHash<QString,QString> getChoosenValues();
    void clear();
private:
    Ui::findValueDialog * ui;
    xmlCategoriesParser * m_xmlParser;
    QSqlDatabase m_db;
    double m_modifier;
    QHash<QString,QString> m_hashValuesChoosen;
    void initialize();
    void fillComboCategories();
    bool tableViewIsFull(QAbstractItemModel * model);
    void enableShowNextTable();
    
private slots:
    void fillListViewValues(const QString & comboItem);
//    void chooseValue(const QModelIndex& index);
    void chooseValue();
    void deleteValue();
//    void supprItemChoosen(QListWidgetItem * item);
    void on_lineEditFilter_textChanged(const QString & text);
    void showNext();
    void setModifSpinBox(QWidget*,QWidget*);
    void setModifier(double);
};


#endif
