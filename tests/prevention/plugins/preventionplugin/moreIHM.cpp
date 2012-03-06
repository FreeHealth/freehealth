#include "moreIHM.h"

MoreIHM::MoreIHM(TreeViewOfPrevention * parent,QModelIndex index){
    setupUi(this);
    QModelIndex parentIndex = parent->model()->parent(index);
    QHash<int,QVariant> hashOfItems;
    hashOfItems = parent->model()->childs(parentIndex);
    QStringList listForTheCombo;
    for (int i = 0; i < hashOfItems.size(); i += 1)
    {
    	  listForTheCombo << hashOfItems.value(i).toString();
        }
    comboItemsSameParent->addItems(listForTheCombo);
    connect(comboItemsSameParent,SIGNAL(activated(int)),this,SLOT(showDocumentAccordingToComboChoice(int)));
        
}

MoreIHM::~MoreIHM(){}

void MoreIHM::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton){
        blockSignals(true);
        emit closeMoreWidget();
        blockSignals(false);
        }

    if(event->button() == Qt::LeftButton) {
        blockSignals(false);
        MoreIHM::mouseReleaseEvent(event);
        
    }
}

void MoreIHM::closeMoreWidget(){
    emit close();
}

void MoreIHM::showDocumentAccordingToComboChoice(int item){}








