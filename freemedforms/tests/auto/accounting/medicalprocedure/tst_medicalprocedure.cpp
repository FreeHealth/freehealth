#include <QString>
#include <QtTest>
#include "../../../../../plugins/account2plugin/models/medicalproceduremodel.h"

class tst_MedicalProcedure : public QObject
{
    Q_OBJECT

public:
    tst_MedicalProcedure();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCreate();
    void testCreate_data();
};

tst_MedicalProcedure::tst_MedicalProcedure()
{
}

void tst_MedicalProcedure::initTestCase()
{
}

void tst_MedicalProcedure::cleanupTestCase()
{
}

void tst_MedicalProcedure::testCreate()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

void tst_MedicalProcedure::testCreate_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

QTEST_APPLESS_MAIN(tst_MedicalProcedure)

#include "tst_medicalprocedure.moc"
