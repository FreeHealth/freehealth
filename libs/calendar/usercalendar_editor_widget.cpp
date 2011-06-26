#include "usercalendar_editor_widget.h"
#include "ui_usercalendar_editor_widget.h"

using namespace Calendar;

UserCalendarEditorWidget::UserCalendarEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserCalendarEditorWidget)
{
    ui->setupUi(this);
}

UserCalendarEditorWidget::~UserCalendarEditorWidget()
{
    delete ui;
}

void UserCalendarEditorWidget::setModel(AbstractCalendarModel *model)
{}

void UserCalendarEditorWidget::setUserCalendar(const Calendar::UserCalendar &calendar)
{}

void UserCalendarEditorWidget::submit()
{}

UserCalendar UserCalendarEditorWidget::userCalendar() const
{}

void UserCalendarEditorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
