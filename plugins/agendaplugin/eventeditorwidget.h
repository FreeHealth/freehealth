#ifndef EVENTEDITORWIDGET_H
#define EVENTEDITORWIDGET_H

#include <QWidget>


namespace Agenda {
class ICalendarEvent;
class IUserCalendar;

namespace Internal {
class EventEditorWidgetPrivate;
}

class EventEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EventEditorWidget(QWidget *parent = 0);
    ~EventEditorWidget();

    void clear();

    void setCalendarEvent(Agenda::ICalendarEvent *event);
    Agenda::ICalendarEvent *calendarEvent() const;

    void setAvailableUserCalendar(const QList<IUserCalendar *> &userCals);

public Q_SLOTS:
    void submit();

private Q_SLOTS:
    void on_selectIconButton_clicked();
    void changeDuration(const int comboIndex);

protected:
    void changeEvent(QEvent *e);

private:
    Internal::EventEditorWidgetPrivate *d;
};

}  // End namespace Agenda

#endif // EVENTEDITORWIDGET_H
