#include "mainwindow.h"

#include <agendaplugin/nextavailabiliystepviewer.h>
#include <utils/global.h>

#include <QDebug>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qWarning();
    ui->setupUi(this);
    m_Next = new Agenda::Internal::NextAvailabiliyManager;

    // Calendar Params
    int defaultDuration = 15;

    using namespace Agenda::Internal;
    QDateTime startTestingDate = Utils::roundDateTime(QDateTime::currentDateTime(), 30);

    qWarning() << "*** Test date to rect transformation";
    QDate d(2011,11,21);
    QDateTime dt(QDateTime(d, QTime(9,0)));
    QRect r = NextAvailabiliyManager::dateToRect(QDateTime(d, QTime(9,0)), 8*60);
    qWarning() << "     test rectToDate (test="
               << dt.toString() << dt.addSecs(8*60*60).toString() << ")\n                           "
               << NextAvailabiliyManager::rectToDateStart(r).toString() << NextAvailabiliyManager::rectToDateEnd(r).toString();

    // Create fake availabilities
    //  Now for one hour
    avs << NextAvailabiliyManager::dateToRect(startTestingDate, 60);

    // Test minutes to nextAvail
    QDateTime test = startTestingDate.addSecs(2*60*60);
    QDateTime dateShouldBe = NextAvailabiliyManager::rectToDateStart(avs.at(0)).addDays(7);
    qWarning();
    qWarning() << "*** Test minutes to next availability: \n"
                  "    testDate" << test <<
                  "\n    firstAv " << NextAvailabiliyManager::rectToDateStart(avs.at(0)) <<
                  "\n    shouldBe" << dateShouldBe << (test.secsTo(dateShouldBe)/60);
    int testResult = NextAvailabiliyManager::minutesToNextAvailability(avs, NextAvailabiliyManager::dateToRect(test, 1));
    qWarning() << QString("     minutesToNextAvail (%1)")
                  .arg(test.secsTo(dateShouldBe)/60)
               << testResult
               << test.addSecs(testResult*60);

    // For minutesToNextAvail computation add one avail 4 hours after now
    avs << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(4*60*60), 60);
    // For minutesToNextAvail computation add one avail 9.5 hours after now
    avs << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(9.5*60*60), 60);
    test = startTestingDate.addSecs(4.5*60*60);
    dateShouldBe = NextAvailabiliyManager::rectToDateStart(avs.at(2));
    qWarning();
    qWarning() << "*** Test minutes to next availability: \n"
                  "    testDate" << test <<
                  "\n    firstAv " << NextAvailabiliyManager::rectToDateStart(avs.at(2)) <<
                  "\n    shouldBe" << dateShouldBe << (test.secsTo(dateShouldBe)/60);
    testResult = NextAvailabiliyManager::minutesToNextAvailability(avs, NextAvailabiliyManager::dateToRect(test, 1));
    qWarning() << QString("     minutesToNextAvail (%1)")
                  .arg(test.secsTo(dateShouldBe)/60)
               << testResult
               << test.addSecs(testResult*60);


    // For minutesToNextAvail
    avs << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(8*24*60*60), 60);
    test = startTestingDate.addSecs(9.6*60*60);
    dateShouldBe = NextAvailabiliyManager::rectToDateStart(avs.at(3));
    qWarning();
    qWarning() << "*** Test minutes to next availability: \n"
                  "    testDate" << test <<
                  "\n    firstAv " << NextAvailabiliyManager::rectToDateStart(avs.at(2)) <<
                  "\n    shouldBe" << dateShouldBe << (test.secsTo(dateShouldBe)/60);
    testResult = NextAvailabiliyManager::minutesToNextAvailability(avs, NextAvailabiliyManager::dateToRect(test, 1));
    qWarning() << QString("     minutesToNextAvail (%1)")
                  .arg(test.secsTo(dateShouldBe)/60)
               << testResult
               << test.addSecs(testResult*60);
    qWarning() << "-----";

//    return;

    //  Monday : 09:00->18:00
    avs << NextAvailabiliyManager::dateToRect(QDateTime(QDate(2011,11,21), QTime(9,0)), 8*60);
    //  Tuesday : 09:00->18:00
    avs << NextAvailabiliyManager::dateToRect(QDateTime(QDate(2011,11,24), QTime(9,0)), 8*60);
    qSort(avs.begin(), avs.end(), NextAvailabiliyManager::rectLessThan);


    // Create fake appointements
    // before the current date
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(-10*60*60), defaultDuration);
    // inside availabilities
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(4.1*60*60), defaultDuration);
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(4.25*60*60), defaultDuration);
    // after the current date
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(6*60), defaultDuration);
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(120*60), defaultDuration);
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(180*60), defaultDuration);
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(240*60), defaultDuration);
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(5*60*60), defaultDuration);
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(7*60*60), defaultDuration);
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(9*60*60), defaultDuration);
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(11*60*60), defaultDuration);
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(17*60*60), defaultDuration);
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(28*60*60), defaultDuration);
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(32*60*60), defaultDuration);
    aps << NextAvailabiliyManager::dateToRect(startTestingDate.addSecs(42*60*60), defaultDuration);
    qSort(aps.begin(), aps.end(), NextAvailabiliyManager::rectLessThan);
    aps << QRect();

    // populate text
    QString tmp;
    for(int i = 0 ; i < avs.count(); ++i) {
        QRect r = avs.at(i);
        tmp += QString("<table><tr><td>%1</td><td>%2</td></tr></table>")
                .arg(NextAvailabiliyManager::rectToDateStart(r).toString())
                .arg(NextAvailabiliyManager::rectToDateEnd(r).toString());
    }
    ui->availText->setHtml(tmp);

    tmp.clear();
    for(int i = 0 ; i < aps.count(); ++i) {
        QRect r = aps.at(i);
        tmp += QString("<table><tr><td>%1</td><td>%2</td></tr></table>")
                .arg(NextAvailabiliyManager::rectToDateStart(r).toString())
                .arg(NextAvailabiliyManager::rectToDateEnd(r).toString());
    }
    ui->computText->setHtml(tmp);

    ui->widget->setAvailabilities(avs);
    ui->widget->setAppointements(aps);

    m_Next->setAvaibilitiesToRect(avs);

    QDateTime start = startTestingDate;
//    QDateTime start = QDateTime(QDate(2011,11,22), QTime(00,00));

    int appointementId = 0;
    int numberOfEvents = 50;
    int limitComputation = 10000;//224640; // 10min, 6day a week, 12hours a day :: 1 full year == 12*6 *10 *6 *52 == 224640
    while (free.count() < numberOfEvents) {
        --limitComputation;
        if (limitComputation <= 0)
            break;

        QList<QDateTime> dates = m_Next->nextAvailableTime(start, defaultDuration, defaultDuration, aps.at(appointementId), numberOfEvents-free.count());
        for(int i = 0; i < dates.count(); ++i) {
            free.append(NextAvailabiliyManager::dateToRect(dates.at(i), defaultDuration));
        }

        qWarning() << dates << m_Next->hasReachedNextAppointement();

        if (m_Next->hasReachedNextAppointement()) {
            start = m_Next->requestingNewAppointementLaterThan();
            qWarning() << "NEXT END" << start;
            ++appointementId;
        }
        if (appointementId == aps.count())
            break;
    }

    qWarning() << "FREE" << free.count();
    tmp.clear();
    for(int i = 0 ; i < free.count(); ++i) {
        QRect r = free.at(i);
        tmp += QString("<table><tr><td>%1</td><td>%2</td></tr></table>")
                .arg(NextAvailabiliyManager::rectToDateStart(r).toString())
                .arg(NextAvailabiliyManager::rectToDateEnd(r).toString());
    }
    ui->freeText->setHtml(tmp);

    ui->widget->addFoundFreeTime(free);

    resize(600, 1000);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_Next;
}
