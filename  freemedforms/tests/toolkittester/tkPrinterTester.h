#ifndef TKPRINTERTESTER_H
#define TKPRINTERTESTER_H

#include <QMainWindow>
class tkPrinterPreviewer;


class tkPrinterTester
{
public:
    tkPrinterTester( QMainWindow *w );
    void test1();
    void previewerTest();

private:
    QMainWindow *w;
    tkPrinterPreviewer *previewer;
};

#endif // TKPRINTERTESTER_H
