#ifndef NEXTAVAILABILIYSTEPVIEWER_H
#define NEXTAVAILABILIYSTEPVIEWER_H

#include <QWidget>
#include <QDateTime>
#include <QRect>

namespace Agenda {
namespace Internal {
namespace Ui {
    class NextAvailabiliyStepViewer;
}

class NextAvailabiliyStepViewer : public QWidget
{
    Q_OBJECT

public:
    explicit NextAvailabiliyStepViewer(QWidget *parent = 0);
    ~NextAvailabiliyStepViewer();

    void setAvailabilities(const QList<QRect> &avs);
    void setAppointements(const QList<QRect> &avs);

    void addFoundFreeTime(const QList<QRect> &free);

    void drawStep();

//    QSize sizeHint() const;

private:
    void paintEvent(QPaintEvent *);

private:
    Ui::NextAvailabiliyStepViewer *ui;
    QList<QRect> avs;
    QList<QRect> aps;
    QList<QRect> free;
};


class NextAvailabiliyManager
{
public:
    NextAvailabiliyManager() : m_ReachedNextAppointement(false) {}
    ~NextAvailabiliyManager() {}

    static bool rectLessThan(const QRect &r1, const QRect &r2);
    static QRect dateToRect(const QDateTime &date, const int durationInMinutes);
    static QRect dateToRect(const QDateTime &begin, const QDateTime &end);
    static QDateTime rectToDateStart(const QRect &rect);
    static QDateTime rectToDateEnd(const QRect &rect);
    static bool isInAvailabilities(const QList<QRect> &avList, const QRect &testDate);
    static int minutesToNextAvailability(const QList<QRect> &avList, const QRect &testDate);

    void setAvaibilitiesToRect(const QList<QRect> &avs);

    QList<QDateTime> nextAvailableTime(const QDateTime &startSearch,
                                       const int durationInMinutes,
                                       const int calendarDurationInMinutes,
                                       const QRect &nextAppointement,
                                       const int numberOfDates);

    bool hasReachedNextAppointement() const {return m_ReachedNextAppointement;}
    QDateTime requestingNewAppointementLaterThan() const {return m_NeedLaterThan;}

private:
    QList<QRect> avs;
    bool m_ReachedNextAppointement;
    QDateTime m_NeedLaterThan;
};

}  // End namespace Internal
}  // End namespace Agenda

#endif // NEXTAVAILABILIYSTEPVIEWER_H
