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
#ifndef RECEIPTSMANAGER_H
#define RECEIPTSMANAGER_H

#include <accountplugin/account_exporter.h>

#include <QtCore>
#include <QHash>
#include <QVariant>
#include <QString>
#include <QStringList>

class ACCOUNT_EXPORT receiptsManager : public QObject
{
  Q_OBJECT
  public :
  receiptsManager();
  ~receiptsManager();
  QHash<int,QString> getPercentages();
  QHash<QString,QVariant> getParametersDatas(QString & userUid , const QString & table);//name,uid
  QHash<QString,QVariant> getHashOfSites();
  QHash<QString,QVariant> getHashOfInsurance();
  QHash<QString,QVariant> getDistanceRules();
  QHash<QString,QVariant> getHashOfThesaurus();
  QHash<QString,QString> getPreferentialActFromThesaurus(const QString & userUuid);
  bool getPreferedValues();
  QVariant getPreferedDistanceRule();
  //QString createTablesAndFields();
  //bool writeAllDefaultsValues();
  QStringList getChoiceFromCategories(QString &);
  QString getStringPerferedActAndValues(const QString & act);
  QVariant m_preferedInsurance;
  QVariant m_preferedSite;
  QVariant m_preferedDistanceRule;
  QVariant m_preferedDistanceValue;
  QVariant m_preferedInsuranceUid;
  QString m_rbmReq;

};

#endif
