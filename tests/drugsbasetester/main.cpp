#include <QApplication>
#include <QWidget>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QVariant>

#include <QDebug>


static bool createConnection()
{
    QString file = qApp->applicationDirPath() + QDir::separator() + "drugs.mdb";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(QFileInfo(file).absoluteFilePath());

    bool ok = db.open();
    if (!ok)
    {
        qDebug() << db.lastError();
        return false;
    }
}

static void read()
{
    QString file = qApp->applicationDirPath() + QDir::separator() + "drugs.mdb";
    QString table = "exemple";
    QSqlDatabase m_db;

    QString dataBaseString = "DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + file;
    m_db.setDatabaseName(dataBaseString);
    if (m_db.open())
    {
        QString command = "SELECT Support FROM " + table + " GROUP BY Support;";

        QStringList list;

        QSqlQuery resultat = m_db.exec(command);
        while (resultat.next())
        {
            list.append(resultat.value(0).toString());
        }
        m_db.close();
    }
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    createConnection();
    return app.exec();
}




