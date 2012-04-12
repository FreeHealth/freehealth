#ifndef MOREIHM_H
#define MOREIHM_H

#include "preventIO.h"
#include "preventIHM.h"

#include "ui_moreIHM.h"
#include <QWidget>
#include <QMouseEvent>

class TreeViewOfPrevention;

class MoreIHM : public QWidget , Ui::MoreWidget{
    Q_OBJECT
    public:
        MoreIHM(QObject *parent, TreeViewOfPrevention * treeView , QModelIndex index = QModelIndex());
        ~MoreIHM();
    
    private Q_SLOTS:
        void showDocumentAccordingToComboChoice(int);
    private:
        void closeMoreWidget();
        //void mouseReleaseEvent(QMouseEvent *event);
};

#endif

