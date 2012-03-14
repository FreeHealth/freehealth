#include "preventIHM.h"
#include "treeviewdelegate.h"

#include <QLabel>
#include <QDateEdit>
#include <QMessageBox>


///////////////////////////////////////////////////////
// TREEVIEWOFPREVENTION ///////////////////////////////
///////////////////////////////////////////////////////

TreeViewOfPrevention::TreeViewOfPrevention(QWidget * parent){
    m_deleteValue = new QAction(trUtf8("Delete this value."),this);
    m_showMore = new QAction(trUtf8("More."),this);
    m_addValue = new QAction(trUtf8("Add a value."),this);
    connect(m_deleteValue,SIGNAL(triggered(bool)),this,SLOT(deleteItem(bool)));
    connect(m_showMore,SIGNAL(triggered(bool)),this,SLOT(showMore(bool)));
    connect(m_addValue,SIGNAL(triggered(bool)),this,SLOT(addAValue(bool)));
    
}
TreeViewOfPrevention::~TreeViewOfPrevention(){}

void TreeViewOfPrevention::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton){
        if(isChild()){
            blockSignals(true);
            m_menuRightClic = new QMenu(this);
            m_menuRightClic -> addAction(m_showMore);
            m_menuRightClic-> addAction(m_deleteValue);
            m_menuRightClic->addAction(m_addValue);
            m_menuRightClic->exec(event->globalPos());
            blockSignals(false);
        }

    }
    if(event->button() == Qt::LeftButton) {
        blockSignals(false);
        QTreeView::mouseReleaseEvent(event);
        
    }
}

bool TreeViewOfPrevention::isChild(){
    bool success = true;
    return success;
}

void TreeViewOfPrevention::deleteItem(bool b){
    Q_UNUSED(b);
    QMessageBox msgBox;
    msgBox.setText("Do you want to delete chosen item ?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    QModelIndex index;
    switch(ret){
    case QMessageBox::Yes :
        index = currentIndex();
        deleteItemAccordingToIndex(index);
        break;
    case QMessageBox::No :
        break;
    } 
}

bool TreeViewOfPrevention::deleteItemAccordingToIndex(QModelIndex & index){
    bool success = true;
    QMessageBox::information(0,trUtf8("TODO"),trUtf8("In delete"),QMessageBox::Ok);
    return success;
}

void TreeViewOfPrevention::showMore(bool b){
    Q_UNUSED(b);
    QModelIndex index = currentIndex();
    MoreIHM * more = new MoreIHM(this,index);
    more->show();
}

void TreeViewOfPrevention::addAValue(bool b)
{
    Q_UNUSED(b);
    QModelIndex index = currentIndex();
    QModelIndex parent = model()->parent(index);
    qDebug() << __FILE__ << QString::number(__LINE__) << " index row =" <<  QString::number(index.row());
    qDebug() << __FILE__ << QString::number(__LINE__) << " parent row =" << QString::number(parent.row()) ;
    
    PreventIO io;
    if (io.addAnItemAccordingToIndex(index,parent,this))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "row inserted" ;
        }
    
}

void TreeViewOfPrevention::getModel(VariantItemModel * model){
     m_model = model ;
}

VariantItemModel * TreeViewOfPrevention::model(){
    return m_model;
}


/////////////////////////////////////////////////////////
// PREVENTIHM ///////////////////////////////////////////
/////////////////////////////////////////////////////////

PreventIHM::PreventIHM(QWidget * parent):QWidget(parent){
    setupUi(this);
QStringList headers;
    m_io = new PreventIO;
    headers = m_io->setHeadersDatas() ;
    m_model = m_io->getModel();
    m_modelOfItems = new VariantItemModel(headers,m_model,this);
    m_TreeViewOfPrevention = new TreeViewOfPrevention(this);
    m_vbox = new QVBoxLayout(this);
    m_vbox->addWidget(m_TreeViewOfPrevention);
    setLayout(m_vbox);
    m_TreeViewOfPrevention->setModel(m_modelOfItems);
    m_TreeViewOfPrevention->getModel(m_modelOfItems) ;
    m_TreeViewOfPrevention->header()->hideSection(VariantItemModel::TYPE_OF_ITEM_H);
    m_TreeViewOfPrevention->header()->hideSection(VariantItemModel::PARENT_ITEM_H);
    m_TreeViewOfPrevention->header()->hideSection(VariantItemModel::PARENT_OR_CHILD_H);
    m_TreeViewOfPrevention->header()->hideSection(VariantItemModel::RESULT_H);
    //m_TreeViewOfPrevention->setItemDelegate(new TreeViewDelegate);
    m_TreeViewOfPrevention->setItemDelegateForColumn(VariantItemModel::DATE_DONE_H,new TreeViewDelegate);
    m_TreeViewOfPrevention->setItemDelegateForColumn(VariantItemModel::DATE_NEXT_H,new TreeViewDelegate);
    
    }

PreventIHM::~PreventIHM(){}

