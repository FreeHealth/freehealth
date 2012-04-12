#include "moreIHM.h"

MoreIHM::MoreIHM(QObject * parent,TreeViewOfPrevention * treeView,QModelIndex index){
    setupUi(this);
    setParent(static_cast<QWidget*> (parent));
    setAutoFillBackground(true);
    setWindowFlags(Qt::Window);
    QPoint posOfMainWindow = static_cast<QWidget*>(parent->parent()) ->pos();
    int a = posOfMainWindow.x();
    int b = posOfMainWindow.y();
    const QString comboLabelText = "<html><body><font color=red size=3>"+trUtf8("Choose item")+
                                   "</font></body></html>";
    comboLabel->setText(comboLabelText);
    move(a+50,b+50);
    quitButton->setShortcut(QKeySequence::InsertParagraphSeparator);
    QModelIndex parentIndex = treeView->model()->parent(index);
    QHash<int,QVariant> hashOfItems;
    hashOfItems = treeView->model()->childs(parentIndex);
    QStringList listForTheCombo;
    for (int i = 0; i < hashOfItems.size(); i += 1)
    {
    	  listForTheCombo << hashOfItems.value(i).toString();
        }
    comboItemsSameParent->addItems(listForTheCombo);
    connect(comboItemsSameParent,SIGNAL(activated(int)),this,SLOT(showDocumentAccordingToComboChoice(int)));
    connect(quitButton,SIGNAL(pressed()),this,SLOT(close()));
}

MoreIHM::~MoreIHM(){}

/*void MoreIHM::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton){
        blockSignals(true);
        emit closeMoreWidget();
        blockSignals(false);
        }

    if(event->button() == Qt::LeftButton) {
        blockSignals(false);
        QWidget::mouseReleaseEvent(event);
        
    }
}*/

void MoreIHM::closeMoreWidget(){
    emit close();
}

void MoreIHM::showDocumentAccordingToComboChoice(int item){}








