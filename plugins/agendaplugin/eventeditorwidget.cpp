#include "eventeditorwidget.h"

#include <agendaplugin/icalendarevent.h>
#include <agendaplugin/iusercalendar.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

#include <translationutils/constanttranslations.h>

#include "ui_eventeditorwidget.h"

#include <QIcon>
#include <QStandardItemModel>
#include <QDebug>

using namespace Agenda;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

namespace {
    const int durationDivider = 5; // define a duration each 5 minutes
}

namespace Agenda {
namespace Internal {

    class EventEditorWidgetPrivate
    {
    public:
        EventEditorWidgetPrivate(EventEditorWidget *parent) :
                ui(new Internal::Ui::EventEditorWidget),
                m_Event(0),
                m_UserCalsModel(0),
                q(parent)
        {
        }

        ~EventEditorWidgetPrivate() {}

        void populateDurationCombo()
        {
            ui->durationCombo->clear();
            for(int i = 0; i < (60/durationDivider); ++i) {
                ui->durationCombo->addItem(QString::number(i*5) + " " + tkTr(Trans::Constants::MINUTES));
            }
        }

        void setEventToUi()
        {
//            ui->typeCombo->setCurrentIndex();
            QDateTime start = m_Event->data(ICalendarEvent::DateStart).toDateTime();
            QDateTime end = m_Event->data(ICalendarEvent::DateEnd).toDateTime();
            int durationInMinutes = start.secsTo(end) / 60;
            ui->durationCombo->setCurrentIndex(durationInMinutes / durationDivider);
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
        }

        void submit()
        {
            if (!m_Event)
                return;
            m_Event->setData(ICalendarEvent::DateStart, QDateTime(ui->startDate->date(), ui->startTime->time()));
            m_Event->setData(ICalendarEvent::DateEnd, QDateTime(ui->endDate->date(), ui->endTime->time()));
            m_Event->setData(ICalendarEvent::TextualSite, ui->location->text());
            m_Event->setData(ICalendarEvent::IsBusy, ui->busyCheck->isChecked());
            m_Event->setData(ICalendarEvent::IsPrivate, ui->privateCheck->isChecked());
            m_Event->setData(ICalendarEvent::Password, ui->password->text());
            m_Event->setData(ICalendarEvent::Label, ui->eventLabel->text());
            m_Event->setData(ICalendarEvent::FullContent,ui->fullInfo->toHtml());
//            m_Event->setData(ICalendarEvent::ThemedIcon, QString());
            // get user calendar
            if (ui->calendarCombo->currentIndex() <= m_UserCals.count()) {
                IUserCalendar *ucal = m_UserCals.at(ui->calendarCombo->currentIndex());
                m_Event->setData(ICalendarEvent::DbOnly_CalId, ucal->calendarId());
            }
        }

    public:
        Ui::EventEditorWidget *ui;
        Agenda::ICalendarEvent *m_Event;
        QList<IUserCalendar *> m_UserCals;
        QStandardItemModel *m_UserCalsModel;

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
    d->populateDurationCombo();
    connect(d->ui->durationCombo, SIGNAL(activated(int)), this, SLOT(changeDuration(int)));
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

    // create the model
    if (d->m_UserCalsModel) {
        delete d->m_UserCalsModel;
        d->m_UserCalsModel = 0;
    }
    d->m_UserCalsModel = new QStandardItemModel(this);
    QStandardItem *root = d->m_UserCalsModel->invisibleRootItem();
    int defaultRow = -1;
    for(int i = 0; i < userCals.count(); ++i) {
        root->appendRow(userCals.at(i)->toStandardItem());
        if (userCals.at(i)->data(IUserCalendar::IsDefault).toBool()) {
            defaultRow = i;
        }
    }
    d->ui->calendarCombo->setModel(d->m_UserCalsModel);
    d->ui->calendarCombo->setCurrentIndex(defaultRow);
}

/** Submit UI changes to the internal Agenda::ICalendarEvent \sa calendarEvent(), setCalendarEvent() */
void EventEditorWidget::submit()
{
    d->submit();
}

void EventEditorWidget::on_selectIconButton_clicked()
{}

void EventEditorWidget::changeDuration(const int comboIndex)
{
    QTime end = d->ui->startTime->time();
    end = end.addSecs(comboIndex * durationDivider * 60);
    d->ui->endTime->setTime(end);
}

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
