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
  QStringList getComboBoxesDatas(const QString &,const QString&);
  QString createTablesAndFields();
  bool writeAllDefaultsValues();
  QStringList getChoiceFromCategories(QString &);
  QString m_rbmReq;

};

#endif
