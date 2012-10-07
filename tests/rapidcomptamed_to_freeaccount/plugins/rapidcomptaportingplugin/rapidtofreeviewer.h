#ifndef RAPIDTOFREEVIEWER_H
#define RAPIDTOFREEVIEWER_H

#include "ui_rapidtofree.h"

#include <QWidget>


class RapidToFreeViewer:public QWidget,public Ui::RapidToFreeWidget
{
    Q_OBJECT
public:
    RapidToFreeViewer (QObject * parent = 0);
    virtual ~RapidToFreeViewer ();

protected slots:
    void getAndSetDatas();

private:
    QStringList rapidcomptamedUsers();
};

#endif

