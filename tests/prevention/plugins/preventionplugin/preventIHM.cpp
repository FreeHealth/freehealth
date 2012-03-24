#include "preventIHM.h"
#include "treeviewdelegate.h"

#include <QLabel>
#include <QDateEdit>
#include <QMessageBox>

// Block pour l'utilisation des pixmaps du theme de FreeMedForms
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

// Pour récupérer le chemin vers un pixmap
//     theme()->iconFullPath(Core::Constants::ICONABOUT); // sous entendu Core::ITheme::SmallSize
//     theme()->iconFullPath(Core::Constants::ICONABOUT, Core::ITheme::MediumSize);
//     theme()->iconFullPath(Core::Constants::ICONABOUT, Core::ITheme::BigSize);

// Pour récupérer l'icône
//     theme()->icon(Core::Constants::ICONABOUT); // sous entendu Core::ITheme::SmallSize
//     theme()->icon(Core::Constants::ICONABOUT, Core::ITheme::MediumSize);
//     theme()->icon(Core::Constants::ICONABOUT, Core::ITheme::BigSize);


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
    m_survey = new Survey(this);
    m_modelOfItems = new VariantItemModel(headers,m_model,this);
    m_TreeViewOfPrevention = new TreeViewOfPrevention(this);
    m_vbox = new QVBoxLayout(this);
    m_vbox->addWidget(m_TreeViewOfPrevention);
    setLayout(m_vbox);

    m_TreeViewOfPrevention->setModel(m_modelOfItems);
    
    m_TreeViewOfPrevention->header()->hideSection(VariantItemModel::TYPE_OF_ITEM_H);
    m_TreeViewOfPrevention->header()->hideSection(VariantItemModel::PARENT_ITEM_H);
    m_TreeViewOfPrevention->header()->hideSection(VariantItemModel::PARENT_OR_CHILD_H);
    m_TreeViewOfPrevention->header()->hideSection(VariantItemModel::RESULT_H);
    m_TreeViewOfPrevention->setItemDelegateForColumn(VariantItemModel::DATE_DONE_H,new DateEditTreeViewDelegate);
    m_TreeViewOfPrevention->setItemDelegateForColumn(VariantItemModel::DATE_NEXT_H,new DateEditTreeViewDelegate);
    m_TreeViewOfPrevention->setItemDelegateForColumn(VariantItemModel::ICON_H,new ComboTreeViewDelegate);
    changeIconWidget();
    
    connect(m_survey,SIGNAL(iconsReset(const QHash<int,QVariant>&)),this,
                     SLOT(iconsResetIfDateNextOvertaken(const QHash<int,QVariant>&)));
    connect(m_modelOfItems,SIGNAL(layoutChanged()),this,SLOT(changeIconWidget()));
    
    

}

PreventIHM::~PreventIHM(){}

void PreventIHM::changeIconWidget()
{
    for (int row = 0; row < m_modelOfItems->rowCount(); row += 1)
    {
    	  QModelIndex parent = m_modelOfItems->index(row,0);
    	  
    	  for (int underRow = 0; underRow < m_modelOfItems->rowCount(parent); underRow += 1)
    	  {
    	  	 QModelIndex indexChild = m_modelOfItems->index(underRow,VariantItemModel::ICON_H,parent);
    	  	 qDebug() << __FILE__ << QString::number(__LINE__) << " row Child =" 
    	  	          << QString::number(indexChild.row()) ;
    	  	 
    	  	 QString data = m_modelOfItems->data(indexChild,Qt::DisplayRole).toString();
    	  	 qDebug() << __FILE__ << QString::number(__LINE__) << " indexChild.data().toString() =" 
    	  	          << data ;
    	  	 m_TreeViewOfPrevention->closePersistentEditor(indexChild);
    	  	 m_TreeViewOfPrevention->openPersistentEditor (indexChild);
                }
     }
}

void PreventIHM::iconsResetIfDateNextOvertaken(const QHash<int,QVariant> & hashOfOvertakenItems)
{
    QList<QVariant> listOfId = hashOfOvertakenItems.values();
    for (int i = 0; i < listOfId.size(); i += 1)
    {
    	  QVariant idItem = listOfId[i];
    	  m_modelOfItems->setIconWarning(idItem);
    	  update();
        }
    
}
