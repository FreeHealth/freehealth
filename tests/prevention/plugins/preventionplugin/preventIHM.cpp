#include "preventIHM.h"
#include "IPrevention.h"

#include <preferences/dateOfNextAction.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>

#include <QLabel>
#include <QDateEdit>
#include <QMessageBox>

// Block pour l'utilisation des pixmaps du theme de FreeMedForms
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }

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

TreeViewOfPrevention::TreeViewOfPrevention(QObject * parent){
    m_parent = parent;
    m_io = new PreventIO(parent);
    m_deleteValue = new QAction(trUtf8("Delete this value."),this);
    m_showMore = new QAction(trUtf8("More."),this);
    m_addValue = new QAction(trUtf8("Add a value."),this);
    m_addGroup = new QAction(trUtf8("Add a group."),this);
    m_deleteGroup = new QAction(trUtf8("Delete this group."),this);
    m_defineNextDate = new QAction(trUtf8("Add a rule."),this);
    connect(m_deleteValue,SIGNAL(triggered(bool)),this,SLOT(deleteItem(bool)));
    connect(m_showMore,SIGNAL(triggered(bool)),this,SLOT(showMore(bool)));
    connect(m_addValue,SIGNAL(triggered(bool)),this,SLOT(addAValue(bool)));
    connect(m_addGroup,SIGNAL(triggered(bool)),this,SLOT(addAGroup(bool)));
    connect(m_deleteGroup,SIGNAL(triggered(bool)),this,SLOT(deleteGroup(bool)));
    connect(m_defineNextDate,SIGNAL(triggered(bool)),this,SLOT(addPreventionPreferences(bool)));
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
            m_menuRightClic->addAction(m_addGroup);
            m_menuRightClic->addAction(m_deleteGroup);
            m_menuRightClic->addAction(m_defineNextDate);
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

void TreeViewOfPrevention::addPreventionPreferences(bool b)
{
    Q_UNUSED(b);
    Prevention::Internal::NextAction * a = new Prevention::Internal::NextAction(this);
    a->show();
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
    QModelIndex indexInside = currentIndex();
    QModelIndex parent = model()->parent(indexInside);
    if (m_model->deleteItemAccordingToIndex(indexInside,parent,this))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "succeed in deleting row" ;
        }
    return success;
}

void TreeViewOfPrevention::showMore(bool b){
    Q_UNUSED(b);
    QModelIndex index = currentIndex();
    MoreIHM * more = new MoreIHM(m_parent,this,index);
    more->show();
}

void TreeViewOfPrevention::addAValue(bool b)
{
    Q_UNUSED(b);
    QModelIndex index = currentIndex();
    QModelIndex parent = model()->parent(index);
    if (m_model->addAnItemAccordingToIndex(index,parent,this))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "row inserted" ;
        }
}

void TreeViewOfPrevention::addAGroup(bool b)
{
    Q_UNUSED(b);
    QModelIndex index = currentIndex();
    QModelIndex parent = model()->parent(index);
    if (m_model->addAGroupItem(index,parent,this))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "group inserted" ;
        }
}

void TreeViewOfPrevention::deleteGroup(bool b)
{
    Q_UNUSED(b);
    QMessageBox msgBox;
    msgBox.setText("Do you want to delete this group ?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    QModelIndex index = currentIndex();
    if (index.parent().isValid())
    {
    	QMessageBox::warning(0,trUtf8("warning"),trUtf8("It is not a group."),QMessageBox::Ok);
    	return;    	  
        }
    else
    {
        switch(ret){
            case QMessageBox::Yes :
                m_model->deleteGroupOfItems(index);
                break;
            case QMessageBox::No :
                break;
            default :
                break;    
            }
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
    m_survey = new Survey(this);
    m_io = new PreventIO(this);
    m_vbox = new QVBoxLayout(this);
    m_vboxSplitter = new QSplitter(this);
    m_Editor = new QTextEdit(this);
    m_TreeViewOfPrevention = new TreeViewOfPrevention(this); 
    m_modelOfItems = m_io->getVariantItemModel();
    m_TreeViewOfPrevention->setModel(m_modelOfItems);
    m_TreeViewOfPrevention->getModel(m_modelOfItems);
    m_TreeViewOfPrevention->setItemDelegateForColumn(PreventionEngine::IPreventionEngine::DATE_DONE_H,new DateEditTreeViewFirstDelegate(m_io));
    m_TreeViewOfPrevention->setItemDelegateForColumn(PreventionEngine::IPreventionEngine::DATE_NEXT_H,new DateEditTreeViewDelegate(m_io));
    m_TreeViewOfPrevention->setItemDelegateForColumn(PreventionEngine::IPreventionEngine::ICON_H,new ComboTreeViewDelegate);
    m_TreeViewOfPrevention->setItemDelegateForColumn(PreventionEngine::IPreventionEngine::ITEM_H,new ComboTreeViewItemDelegate(m_io));
    m_TreeViewOfPrevention->header()->hideSection(PreventionEngine::IPreventionEngine::TYPE_OF_ITEM_H);
    m_TreeViewOfPrevention->header()->hideSection(PreventionEngine::IPreventionEngine::PARENT_ITEM_H);
    m_TreeViewOfPrevention->header()->hideSection(PreventionEngine::IPreventionEngine::PARENT_OR_CHILD_H);
    m_TreeViewOfPrevention->header()->hideSection(PreventionEngine::IPreventionEngine::RESULT_H);
    m_TreeViewOfPrevention->header()->hideSection(PreventionEngine::IPreventionEngine::PATIENT_UID_H);
    m_TreeViewOfPrevention->header()->hideSection(PreventionEngine::IPreventionEngine::USER_UID_H);
    m_TreeViewOfPrevention->header()->resizeSection(PreventionEngine::IPreventionEngine::ITEM_H,200);
    m_vboxSplitter->addWidget(m_TreeViewOfPrevention);
    m_vboxSplitter->addWidget(m_Editor);
    m_vbox->addWidget(m_vboxSplitter);
    setLayout(m_vbox);
    changeIconWidget();
    connect(m_survey,SIGNAL(iconsReset(const QHash<int,QVariant>&)),this,
                     SLOT(iconsResetIfDateNextOvertaken(const QHash<int,QVariant>&)));
    connect(m_modelOfItems,SIGNAL(layoutChanged()),this,SLOT(changeIconWidget()));
    connect(m_TreeViewOfPrevention,SIGNAL(clicked(const QModelIndex&)),this,SLOT(showDocument(const QModelIndex&)));
}

PreventIHM::~PreventIHM(){}

void PreventIHM::changeIconWidget()
{
    VariantItemModel * modelOfItems = m_TreeViewOfPrevention->model();//m_modelOfItems;
    for (int row = 0; row < modelOfItems->rowCount(); row += 1)
    {
    	  QModelIndex parent = modelOfItems->index(row,0);
    	  
    	  for (int underRow = 0; underRow < modelOfItems->rowCount(parent); underRow += 1)
    	  {
    	  	 QModelIndex indexChild = modelOfItems->index(underRow,PreventionEngine::IPreventionEngine::ICON_H,parent);
    	  	 QString data = modelOfItems->data(indexChild,Qt::DisplayRole).toString();
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
        }
}

void PreventIHM::showDocument(const QModelIndex & index)
{
    QString idItem;
    PreventIO io;
    idItem = io.getIdItem(index);
    QString document = io.getDocumentRelativeToIdItem(idItem);
    QTextDocument *textDocument = new QTextDocument (document);
    m_Editor->setDocument(textDocument);
}
