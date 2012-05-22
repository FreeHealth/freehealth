#ifndef WINDOW_H
#define WINDOW_H

#include <QDialog>
#include <QDir>
#include <QMainWindow>

class QComboBox;
class QLabel;
class QPushButton;


class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(QWidget* parent = 0);

private slots:
    void browseDest();

    void about();
    void printStartEnd();


private:

    QPushButton *createButton(const QString &text, const char *member);
    QComboBox *createComboBox(const QString &text = QString());


    QLabel *directoryLabel;


    QPushButton *browseButton2;
    QPushButton *OKButton;

    QDir currentDir;

    QComboBox *destinationComboBox;
    QLabel *destinationLabel;

    //About
    void createActions();
    void createMenus();
    QMenu *helpMenu;
    QAction *aboutAction;

};

#endif

