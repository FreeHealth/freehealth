#ifndef CONNEXION_H
#define CONNEXION_H

bool connexion(){
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "freeaccount");
            db.setHostName("localhost");
            db.setDatabaseName("account.db");
            db.setUserName("root");
            db.setPassword("");
            if(!db.open() && !db.isOpen()) return false;
            return true;
}

bool createDatabase(){
    qDebug() <<  __FILE__ << QString::number(__LINE__) ;
    QSqlDatabase db = QSqlDatabase::database("QSQLITE", "freeaccount");
    QString s = "CREATE DATABASE account.db";
    QSqlQuery q(db);
    if(!q.exec(s)) return false;
    return true;
}

#endif
