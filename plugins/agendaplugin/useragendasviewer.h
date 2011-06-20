#ifndef USERAGENDASVIEWER_H
#define USERAGENDASVIEWER_H

#include <QWidget>


namespace Agenda {
class IUserCalendar;

namespace Internal {
class UserAgendasViewerPrivate;

class UserAgendasViewer : public QWidget
{
    Q_OBJECT

public:
    enum AgendaOwner {
        OwnAgendas = 0,
        DelegatedAgendas
    };

    explicit UserAgendasViewer(QWidget *parent = 0);
    ~UserAgendasViewer();

    void setUserCalendar(const QList<IUserCalendar *> &user, const AgendaOwner owner = OwnAgendas);

protected:
    void changeEvent(QEvent *e);

private:
    UserAgendasViewerPrivate *d;
};

}  // End namespace Internal
}  // End namespace Agenda

#endif // USERAGENDASVIEWER_H
