#ifndef PMHMODE_H
#define PMHMODE_H

#include <coreplugin/modemanager/basemode.h>

#include <QObject>

namespace PMH {
namespace Internal {
namespace Ui {
class PmhMode;
}

class PmhMode : public Core::BaseMode
{
    Q_OBJECT
public:
    explicit PmhMode(QObject *parent = 0);
    ~PmhMode();

private:
    Ui::PmhMode *ui;
};

}  // End namespace Internal
}  // End namespace PMH

#endif // PMHMODE_H

