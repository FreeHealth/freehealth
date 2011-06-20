#include "useragendasviewer.h"
#include "iusercalendar.h"
#include "icalendarevent.h"
#include "agendabase.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>

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

    void populateCalendarWithCurrentWeek()
    {
        qWarning() << Q_FUNC_INFO;

        // get events from database
        CalendarEventQuery query;
        query.setDateRangeForCurrentWeek();
        m_Events = base()->getCalendarEvents(query);

        qWarning() << m_Events.count();

        // Create calendar items
        Calendar::AbstractCalendarModel *model = ui->calendarViewer->model();
        for(int i = 0; i < m_Events.count(); ++i) {
            ICalendarEvent *event = m_Events.at(i);
            Calendar::CalendarItem item = model->insertItem(event->data(ICalendarEvent::DateStart).toDateTime(), event->data(ICalendarEvent::DateEnd).toDateTime());
            item.setTitle(event->data(ICalendarEvent::Label).toString());
//            item.setDescription(event->data(ICalendarEvent::FullContent).toString());
            model->setItemByUid(item.uid(), item);
            m_UidToListIndex.insert(item.uid(), i);
        }
    }


public:
    Ui::UserAgendasViewer *ui;
    QStandardItemModel *m_UserCalsModel;
    QList<ICalendarEvent *> m_Events;
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
    d->ui->userFullNameLabel->setText(user()->value(Core::IUser::FullName).toString());
}

UserAgendasViewer::~UserAgendasViewer()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void UserAgendasViewer::setUserCalendar(const QList<IUserCalendar *> &userCals, const AgendaOwner owner)
{
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
    d->ui->availableAgendasCombo->setModel(d->m_UserCalsModel);
    d->ui->availableAgendasCombo->setCurrentIndex(defaultRow);

    // update events
    d->populateCalendarWithCurrentWeek();
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
