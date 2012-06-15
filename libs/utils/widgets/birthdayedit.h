#ifndef BIRTHDAYEDIT_H
#define BIRTHDAYEDIT_H

#include <QLineEdit>
#include <utils/global_exporter.h>

#include <QDate>
#include <QDateTimeEdit>
#include <QKeyEvent>

namespace Utils {

class UTILS_EXPORT BirthDayEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QDate date READ date WRITE setDate NOTIFY dateChanged USER true)
//    Q_PROPERTY(bool calendarPopup READ calendarPopup WRITE setCalendarPopup)

public:
    explicit BirthDayEdit(QWidget *parent);
    explicit BirthDayEdit(const QDate & date, QWidget *parent = 0);
    ~BirthDayEdit();

    QDate date() const;

    void setDateFormats(QString formats);

private:
    void init(const QDate& date = QDate(), const QDate& maximumDate = QDate(), const QDate& minimumDate = QDate());

signals:
    void dateChanged (const QDate& date);

public slots:
    virtual void clear();
    void setDate(const QDate& date);
    void setDateString(const QString& dateString);

protected slots:
    void updateDisplayText();

private:
    QDate m_date;
    QDate m_maximumDate;
    QDate m_minimumDate;
    QStringList m_dateFormatList;
};

} // end Utils

#endif // BIRTHDAYEDIT_H
