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
            m_Model(new Agenda::CalendarItemModel(parent)),
            q(parent)
    {
    }

    ~UserAgendasViewerPrivate()
    {
        delete ui;
        if (m_Model) {
            delete m_Model;
            m_Model = 0;
        }
    }

    void populateCalendarWithCurrentWeek(Calendar::UserCalendar *calendar)
    {
        ui->calendarViewer->setModel(m_Model);
    }


public:
    Ui::UserAgendasViewer *ui;
    CalendarItemModel *m_Model;
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

    Calendar::UserCalendar cal = d->m_Model->defaultUserCalendar();
    if (cal.isValid()) {
        d->ui->description->setHtml(cal.data(Calendar::UserCalendar::Description).toString());
        d->ui->durationLabel->setText(cal.data(Calendar::UserCalendar::DefaultDuration).toString());
    }

    int width = size().width();
    int third = width/3;
    d->ui->splitter->setSizes(QList<int>() << third << width-third);

    connect(user(), SIGNAL(userChanged()), this, SLOT(userChanged()));

    userChanged();
}

UserAgendasViewer::~UserAgendasViewer()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void UserAgendasViewer::on_availableAgendasCombo_activated(const int index)
{
//    d->populateCalendarWithCurrentWeek(d->m_UserCals.at(index));
}

void UserAgendasViewer::userChanged()
{
    d->ui->userNameLabel->setText(user()->value(Core::IUser::FullName).toString());
    // model is automatocally updated and reseted but the userCalendar combo model
    d->ui->availableAgendasCombo->setModel(d->m_Model->userCalendarComboModel(this));
    d->ui->availableAgendasCombo->setCurrentIndex(d->m_Model->defaultUserCalendarComboModelIndex());
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
