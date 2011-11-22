#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


namespace Agenda {
namespace Internal {
class NextAvailabiliyManager;
}
}

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QList<QRect> avs;
    QList<QRect> aps;
    QList<QRect> free;
    Agenda::Internal::NextAvailabiliyManager *m_Next;
};

#endif // MAINWINDOW_H
