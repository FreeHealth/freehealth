#include "useragendasviewer.h"
#include "agendabase.h"
#include "constants.h"
#include "calendaritemmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>

#include <calendar/usercalendar.h>

#include "ui_useragendasviewer.h"

#include <QStandardItemModel>

using namespace Agenda;
using namespace Internal;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Agenda::Internal::AgendaBase *base() {return Agenda::Internal::AgendaBase::instance();}

namespace Agenda {
namespace Internal {
class UserAgendasViewerPrivate
{
public:
    UserAgendasViewerPrivate(UserAgendasViewer *parent) :
            ui(new Ui::UserAgendasViewer),
            m_UserCalsModel(0),
            q(parent)
    {}

    ~UserAgendasViewerPrivate()
    {
        delete ui;
        qDeleteAll(m_Events);
        m_Events.clear();
    }

    void populateCalendarWithCurrentWeek(Calendar::UserCalendar *calendar)
    {
        qWarning() << Q_FUNC_INFO;
        qDeleteAll(m_Events);
        m_Events.clear();

        // get events from database
//        CalendarEventQuery query;
//        query.setDateRangeForCurrentWeek();
//        query.setCalendarId(calendar->data(Constants::Db_CalId).toInt());
//        m_Events = base()->getCalendarEvents(query);

//        qWarning() << m_Events.count();

        QTime chrono;
        chrono.start();

        // Create calendar items
        QTime t;
        t.start();
        ui->calendarViewer->setModel(new Agenda::CalendarItemModel(q));
//        Calendar::AbstractCalendarModel *model = ui->calendarViewer->model();
//        model->stopEvents();
//        model->clearAll();
//        for(int i = 0; i < m_Events.count(); ++i) {
//            const Calendar::CalendarItem *item = m_Events.at(i);
//            model->setItemByUid(item->uid(), *item);
//            m_UidToListIndex.insert(item->uid(), i);
//        }
//        qWarning() << (chrono.elapsed()/m_Events.count()) << "ms par item créé == "<< chrono.elapsed()<< "ms";

//        model->resumeEvents();

    }


public:
    Ui::UserAgendasViewer *ui;
    QStandardItemModel *m_UserCalsModel;
    QList<Calendar::CalendarItem *> m_Events;
    QList<Calendar::UserCalendar *> m_UserCals;
    QHash<QString, int> m_UidToListIndex;

private:
    UserAgendasViewer *q;
};

}  // End namespace Internal
}  // End namespace Agenda


UserAgendasViewer::UserAgendasViewer(QWidget *parent) :
    QWidget(parent),
    d(new UserAgendasViewerPrivate(this))
{
    d->ui->setupUi(this);
    d->ui->userFullNameLabel->setText("user:");
}

UserAgendasViewer::~UserAgendasViewer()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void UserAgendasViewer::setUserCalendar(const QList<Calendar::UserCalendar *> &userCals, const AgendaOwner owner)
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
        if (userCals.at(i)->data(Calendar::UserCalendar::IsDefault).toBool()) {
            defaultRow = i;
        }
    }
    d->ui->availableAgendasCombo->setModel(d->m_UserCalsModel);
    d->ui->availableAgendasCombo->setCurrentIndex(defaultRow);

    // update events
    if (defaultRow > -1 && defaultRow < userCals.count())
        d->populateCalendarWithCurrentWeek(userCals.at(defaultRow));
}

void UserAgendasViewer::on_availableAgendasCombo_activated(const int index)
{
    d->populateCalendarWithCurrentWeek(d->m_UserCals.at(index));
}

void UserAgendasViewer::changeEvent(QEvent *e)
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
