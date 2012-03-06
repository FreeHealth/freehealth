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
        MoreIHM(TreeViewOfPrevention * parent , QModelIndex index = QModelIndex());
        ~MoreIHM();
    
    private:
        void closeMoreWidget();
        void showDocumentAccordingToComboChoice(int item);
        void mouseReleaseEvent(QMouseEvent *event);
};

#endif

