#ifndef UserCalendarEditorWidget_H
#define UserCalendarEditorWidget_H

#include <QWidget>

namespace Calendar {
class AbstractCalendarModel;
class UserCalendar;

namespace Ui {
    class UserCalendarEditorWidget;
}

class UserCalendarEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserCalendarEditorWidget(QWidget *parent = 0);
    ~UserCalendarEditorWidget();

    void setModel(AbstractCalendarModel *model);
    void setUserCalendar(const Calendar::UserCalendar &calendar);

public Q_SLOTS:
    void submit();
    UserCalendar userCalendar() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::UserCalendarEditorWidget *ui;
    UserCalendar m_Calendar;
};

}  // End namespace Calendar


#endif // UserCalendarEditorWidget_H
