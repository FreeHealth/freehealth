#ifndef RECEIPTSMANAGER_H
#define RECEIPTSMANAGER_H
#include <QtCore>
#include <QtSql>
class receiptsManager : public QObject {
  Q_OBJECT
  public :
  receiptsManager();
  ~receiptsManager();
  QHash<int,QString> getPercentages();
  QHash<QString,QVariant> getParametersDatas(QString & values , const QString & table);//name,uid
  QHash<QString,QVariant> getHashOfSites();
  QHash<QString,QVariant> getHashOfInsurance();
  //QString createTablesAndFields();
  //bool writeAllDefaultsValues();
  QStringList getChoiceFromCategories(QString &);
  QString m_rbmReq;

};

#endif
