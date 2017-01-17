/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef RECEIPTSMANAGER_H
#define RECEIPTSMANAGER_H

#include <accountplugin/account_exporter.h>

#include <QtCore>
#include <QHash>
#include <QVariant>
#include <QString>
#include <QStringList>

class ACCOUNT_EXPORT ReceiptsManager : public QObject
{
  Q_OBJECT
  public :
  ReceiptsManager(QObject *parent = 0);
  ~ReceiptsManager();

  QHash<int,QString> getPercentages();
  QHash<QString,QVariant> getParametersData(QString & userUid , int table);//uid,name
  QHash<QString,QVariant> getHashOfSites();
  QHash<QString,QVariant> getHashOfInsurance();
  QHash<QString,QVariant> getDistanceRules();
  QHash<QString,QVariant> getHashOfThesaurus();
  QHash<QString,QString> getPreferentialActFromThesaurus(const QString & userUuid);
  bool getpreferredValues();
  QVariant getpreferredDistanceRule();
  //QString createTablesAndFields();
  //bool writeAllDefaultsValues();
  QStringList getChoiceFromCategories(QString &);
  QString getStringPerferedActAndValues(const QString & act);
  QVariant m_preferredInsurance;
  QVariant m_preferredSite;
  QVariant m_preferredDistanceRule;
  QVariant m_preferredDistanceValue;
  QVariant m_preferredInsuranceUid;
  QString m_rbmReq;

  //medintux
  bool isMedintuxArg();
  bool isFreaccountIndependant();
  QString getFullName();

};

#endif
