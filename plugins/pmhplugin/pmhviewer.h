#ifndef PMHVIEWER_H
#define PMHVIEWER_H

#include <QWidget>

namespace PMH {
namespace Internal {
namespace Ui {
class PmhViewer;
}
}  // End namespace Internal

class PmhViewer : public QWidget
{
    Q_OBJECT

public:
    explicit PmhViewer(QWidget *parent = 0);
    ~PmhViewer();

protected:
    void changeEvent(QEvent *e);

private:
    Internal::Ui::PmhViewer *ui;
};

}  // End namespace PMH

#endif // PMHVIEWER_H
