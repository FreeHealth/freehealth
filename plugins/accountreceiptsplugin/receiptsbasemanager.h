#ifndef RECEIPTSBASEMANAGER_H
#define RECEIPTSBASEMANAGER_H
#include <QtCore>
#include <QtSql>
class receiptsBaseManager : public QObject {
    Q_OBJECT
public:
    receiptsBaseManager();
    ~receiptsBaseManager();
    QList<QMultiHash<int,QString> > getPercentages();
    QStringList getComboBoxesDatas(const QString &,const QString&);
    QString createTablesAndFields();
    bool writeAllDefaultsValues();
    QStringList getChoiceFromCategories(QString &);
    QString m_rbmReq;
};

#endif
