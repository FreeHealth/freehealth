/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef CHOICEDIALOG_H
#define CHOICEDIALOG_H

#include <accountplugin/account_exporter.h>

#include <QDialog>
#include <QTimer>
#include <QHash>

#include <QStandardItemModel>
#include <QMenu>
#include <QAction>
#include <QTreeView>

namespace Ui {
    class ChoiceDialog;
}

namespace ChoiceActions {
class ActionsTreeView: public QTreeView
{
    Q_OBJECT

public:
    enum Actions{
        DEBTOR = 0,
        DISTRULES,
        SITES
    };
    ActionsTreeView(QWidget *parent);
    ~ActionsTreeView();

    void reset() {QTreeView::reset();}
    void fillActionTreeView();
    QStandardItemModel *treeModel() const {return m_actionsTreeModel;}
    QModelIndex indexWithItem(int row);

private:
    void mousePressEvent(QMouseEvent *event);
    bool deleteItemFromThesaurus(QModelIndex &index);
    bool addpreferredItem(QModelIndex &index);
    bool isChildOfThesaurus();

private Q_SLOTS:
    void choosepreferredValue(bool b);
    void deleteBox(bool b);

private:
    /** \create a pimpl */
    QAction *m_choosepreferredValue;
    QAction *m_deleteThesaurusValue;
    QMenu *m_menuRightClick;
    QStandardItemModel *m_actionsTreeModel;
};
}

class ACCOUNT_EXPORT ChoiceDialog : public QDialog
{
  Q_OBJECT
public:
  enum ReturningModel{
      TYPE_OF_CHOICE = 0,
      PERCENTAGE,
      DEBTOR,
      SITE,
      DISTRULES,
      returningModel_MaxParam
      };
  ChoiceDialog(QWidget *parent = 0,bool roundtrip = false, bool freetext = false, QString preferredValue = QString());
  ~ChoiceDialog();
  double getDistanceNumber(const QString & data);
  int returnChoiceDialog();
  double returnPercentValue();
  //double m_percentValue;
  QList<double> m_listOfPercentValues;
  QStandardItemModel * m_modelChoicePercentDebtorSiteDistruleValues;
  QList<double> listOfPercentValues();
  QStandardItemModel * getChoicePercentageDebtorSiteDistruleModel();
  //free value and text
  QString getFreeText();
  QString getFreeValue();
  QString getFreePayer();
private Q_SLOTS:
  void value(double val);
  void valueUp();
  void valueDown();
  void valueUpStop();
  void valueDownStop();
  void quickPlus();
  void quickLess();
  void beforeAccepted();
  void actionsOfTreeView(const QModelIndex& index);
private:
  Ui::ChoiceDialog * ui;
  QTimer * m_timerUp;
  QTimer * m_timerDown;
  ChoiceActions::ActionsTreeView *m_actionTreeView;
  QVariant m_siteUid;
  QVariant m_insuranceUid;
  QVariant m_insurance;
  double m_distanceRuleValue;
  QString m_distanceRuleType;
  int m_row;
  double m_percent;
  int m_quickInt;
  QHash<int,QString> m_hashPercentages;
private:
  QVariant firstItemchosenAsPreferential(ReturningModel item);

};

#endif
