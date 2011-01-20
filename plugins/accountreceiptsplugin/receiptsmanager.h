#ifndef RECEIPTSMANAGER_H
#define RECEIPTSMANAGER_H
#include <QtCore>
#include <QtSql>
class receiptsManager : public QObject {
  Q_OBJECT
  public :
  receiptsManager();
  ~receiptsManager();
  QList<QMultiHash<int,QString> > getPercentages();
  QHash<QString,QVariant> getParametersDatas(QString & values , const QString & table);//name,uid
  QHash<QString,QVariant> getHashOfSites();
  //QString createTablesAndFields();
  //bool writeAllDefaultsValues();
  QStringList getChoiceFromCategories(QString &);
  QString m_rbmReq;

};

#endif
