#include "eventeditorwidget.h"

#include <agendaplugin/icalendarevent.h>
#include <agendaplugin/iusercalendar.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

#include "ui_eventeditorwidget.h"

#include <QIcon>
#include <QDebug>


using namespace Agenda;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

namespace Agenda {
namespace Internal {
    class EventEditorWidgetPrivate
    {
    public:
        EventEditorWidgetPrivate(EventEditorWidget *parent) :
                ui(new Internal::Ui::EventEditorWidget),
                m_Event(0),
                q(parent)
        {
        }

        ~EventEditorWidgetPrivate() {}

        void setEventToUi()
        {
//            ui->typeCombo->setCurrentIndex();
            QDateTime start = m_Event->data(ICalendarEvent::DateStart).toDateTime();
            QDateTime end = m_Event->data(ICalendarEvent::DateEnd).toDateTime();
            ui->location->setText(m_Event->data(ICalendarEvent::TextualSite).toString());
            ui->startDate->setDate(start.date());
            ui->endDate->setDate(end.date());
            ui->startTime->setTime(start.time());
            ui->endTime->setTime(end.time());
//            ui->durationCombo->setCurrentIndex(-1);
            ui->busyCheck->setChecked(m_Event->data(ICalendarEvent::IsBusy).toBool());
            ui->privateCheck->setChecked(m_Event->data(ICalendarEvent::IsPrivate).toBool());
            ui->password->setText(m_Event->data(ICalendarEvent::Password).toString());
            ui->eventLabel->setText(m_Event->data(ICalendarEvent::Label).toString());
            ui->fullInfo->setText(m_Event->data(ICalendarEvent::FullContent).toString());
            ui->iconLabel->setPixmap(theme()->icon(m_Event->data(ICalendarEvent::ThemedIcon).toString()).pixmap(16, 16));
            qWarning() << m_Event->data(ICalendarEvent::ThemedIcon).toString();
        }


    public:
        Ui::EventEditorWidget *ui;
        Agenda::ICalendarEvent *m_Event;
        QList<IUserCalendar *> m_UserCals;

    private:
        EventEditorWidget *q;
    };
}
}


EventEditorWidget::EventEditorWidget(QWidget *parent) :
    QWidget(parent),
    d(new Internal::EventEditorWidgetPrivate(this))
{
    d->ui->setupUi(this);
}

EventEditorWidget::~EventEditorWidget()
{
    delete d;
}

void EventEditorWidget::clear()
{
    d->ui->typeCombo->setCurrentIndex(-1);
    d->ui->location->clear();
    d->ui->startDate->setDate(QDate::currentDate());
    d->ui->endDate->setDate(QDate::currentDate());
    d->ui->startTime->setTime(QTime::currentTime());
    d->ui->endTime->setTime(QTime::currentTime());
    d->ui->durationCombo->setCurrentIndex(-1);
    d->ui->busyCheck->setChecked(false);
    d->ui->privateCheck->setChecked(false);
    d->ui->password->clear();
    d->ui->eventLabel->clear();
    d->ui->fullInfo->clear();
    d->ui->iconLabel->clear();
}

void EventEditorWidget::setCalendarEvent(Agenda::ICalendarEvent *event)
{
    d->m_Event = event;
    // Clear ui
    clear();
    // Populate ui
    d->setEventToUi();
}

Agenda::ICalendarEvent *EventEditorWidget::calendarEvent() const
{
    return d->m_Event;
}

void EventEditorWidget::setAvailableUserCalendar(const QList<IUserCalendar *> &userCals)
{
    d->m_UserCals = userCals;
}

/** Submit UI changes to the internal Agenda::ICalendarEvent \sa calendarEvent(), setCalendarEvent() */
void EventEditorWidget::submit()
{
}

void EventEditorWidget::on_selectIconButton_clicked()
{}

void EventEditorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
