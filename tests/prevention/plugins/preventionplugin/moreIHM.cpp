#include "moreIHM.h"
//#include "preventionconstants.h"
#include "IPrevention.h"

#include <utils/global.h>

static inline PreventionEngine::IPreventionEngine *preventionEngine() {return PreventionEngine::IPreventionEngine::preventionEnginePtr();}
/*Message Boxes and default dialogs
UTILS_EXPORT void informativeMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
UTILS_EXPORT void warningMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
UTILS_EXPORT bool yesNoMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null, const QPixmap &icon = QPixmap() );
UTILS_EXPORT int withButtonsMessageBox( const QString &text, const QString&infoText, const QString&detail, const QStringList &buttonsText, const QString &title = QString::null, bool withCancelButton = false );
UTILS_EXPORT int withButtonsMessageBox( const QString &text, const QString&infoText, const QString&detail, QMessageBox::StandardButtons buts, QMessageBox::StandardButton defaultButton, const QString &title = QString::null );
UTILS_EXPORT bool okCancelMessageBox( const QString &text, const QString&infoText, const QString&detail = QString::null, const QString &title = QString::null );
UTILS_EXPORT bool functionNotAvailableMessageBox( const QString &functionText );
*/

MoreIHM::MoreIHM(QObject * parent,TreeViewOfPrevention * treeView,QModelIndex index){
    setupUi(this);
    setParent(static_cast<QWidget*> (parent));
    setAutoFillBackground(true);
    setWindowFlags(Qt::Window);
    QPoint posOfMainWindow = static_cast<QWidget*>(parent->parent()) ->pos();
    int a = posOfMainWindow.x();
    int b = posOfMainWindow.y();
    move(a+50,b+50);
    const QString comboLabelText = "<html><body><font color=red size=3>"+trUtf8("Choose item")+
                                   "</font></body></html>";
    comboLabel->setText(comboLabelText);
    //quitButton->setShortcut(QKeySequence::InsertParagraphSeparator);
    quitButton->setDefault(true);
    saveButton->setShortcut(QKeySequence("CTRL+s"));
    QModelIndex parentIndex = treeView->model()->parent(index);
    m_hashItemAndId = treeView->model()->childsAndItems(parentIndex);
    int id = treeView->model()->data(treeView->model()->index(index.row(),PreventionEngine::IPreventionEngine::ID_ITEM_H,parentIndex),Qt::DisplayRole).toInt();
    qDebug() << __FILE__ << QString::number(__LINE__) << " id in More =" << QString::number(id)  ;
    int item = 0;
    QStringList listForTheCombo;
    for (int i = 0; i < m_hashItemAndId.size(); i += 1)
    { 
        QVector<QVariant> data_id = m_hashItemAndId.value(i);
        listForTheCombo << data_id[DATA].toString();
        int idItem = data_id[ID].toInt();
        if (idItem == id)
        {
        	  item = i;
            }
        }
    m_io = new PreventIO(parent);
    m_db = m_io->getDatabase();
    textBrowser->setOpenExternalLinks(true);
    textBrowser->setReadOnly(false);
    showDocumentAccordingToComboChoice(item);
    comboItemsSameParent->addItems(listForTheCombo);
    connect(comboItemsSameParent,SIGNAL(activated(int)),this,SLOT(showDocumentAccordingToComboChoice(int)));
    connect(quitButton,SIGNAL(pressed()),this,SLOT(closeAndQuit()));
    connect(saveButton,SIGNAL(pressed()),this,SLOT(saveTextBrowser()));
    
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

void MoreIHM::showDocumentAccordingToComboChoice(int item)
{
    PreventIO io;    
    qDebug() << __FILE__ << QString::number(__LINE__) << " id in show document =" << QString::number(item)  ;
    QVector<QVariant> data_id = m_hashItemAndId.value(item);
    QString id_item = data_id[ID].toString();
    qDebug() << __FILE__ << QString::number(__LINE__) << " id_item =" << id_item ;
    QString text = io.getDocumentRelativeToIdItem(id_item);
    textBrowser->setText(text);
}

void MoreIHM::closeAndQuit()
{
    if (textBrowser->document()->isModified())
    {
    	int ret = Utils::withButtonsMessageBox ( trUtf8("Do you want to save and close ?"),
		                               QString(),
		                               QString(),
		                               QMessageBox::Ok | QMessageBox::Close,
		                               QMessageBox::Close,
		                               trUtf8("Save and close")) ;	
        switch(ret){
        case QMessageBox::Ok :
            saveTextBrowser();
            close();
            break;
        case QMessageBox::Close :
            close();
            break;
        default :
            break;    
            }    
        }
    else{
        close();
        }
}

void MoreIHM::saveTextBrowser()
{
    PreventIO io(this);
    QString id_item = m_hashIndexAndId.value(comboItemsSameParent->currentIndex());
    io.recordDocument(textBrowser->document(),id_item);
}




