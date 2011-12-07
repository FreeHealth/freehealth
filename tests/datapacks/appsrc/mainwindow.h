#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <datapackutils/serverdescription.h>

#include <QMainWindow>
#include <QNetworkReply>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private Q_SLOTS:
    void slotReadyRead();
    void slotError(QNetworkReply::NetworkError error);

private:
    Ui::MainWindow *ui;
    DataPack::ServerDescription serverDescr;
};

#endif // MAINWINDOW_H
