/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "receiptviewer.h"
#include "receiptsmanager.h"
#include "receiptsio.h"
#include "findreceiptsvalues.h"
#include "choicedialog.h"
#include "distance.h"
#include "freetext.h"

#include "constants.h"

#include "tools.h"

#include "ui_receiptviewer.h"

#include <accountbaseplugin/constants.h>
#include <accountbaseplugin/workingplacesmodel.h>

#include <accountplugin/ledger/ledgerviewer.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/widgets/spinboxdelegate.h>
#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <QAbstractItemModel>
#include <QDebug>
#include <QFrame>
#include <QPushButton>
#include <QKeySequence>
#include <QString>
#include <QBrush>
#include <QColor>

enum { WarnDebugMessage = false };

using namespace ReceiptsConstants;
using namespace Constants;
using namespace Trans::ConstantTranslations;
using namespace Tools;

static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

using namespace InternalAmount;

DisplayModel::DisplayModel(QObject *parent) : QAbstractTableModel(parent)
{
        m_listsOfValuesbyRows = new QVector<QList<QVariant> >;
}

DisplayModel::~DisplayModel(){}

int DisplayModel::rowCount(const QModelIndex &parent ) const
{
    // prevent trees
    if (parent.isValid()) return 0;

    return m_listsOfValuesbyRows->size();
}

int DisplayModel::columnCount(const QModelIndex &parent) const
{
        Q_UNUSED(parent);
        return int(Col_Count);
}

QModelIndex DisplayModel::index ( int row, int column, const QModelIndex & parent) const
{
        return QAbstractTableModel::index(row,column,parent);
}

bool DisplayModel::insertRows( int position, int count, const QModelIndex & parent)
{
        beginInsertRows(parent, position, position+count-1);
        for (int row=0; row < count; row++) {

            QList<QVariant> list;
            for (int j = 0; j < Col_Count; j += 1)
            {
                list << QVariant(0);
            }
            m_listsOfValuesbyRows -> append(list);
        }
        QList<QVariant> listDouble;
        listDouble = m_listsOfValuesbyRows->at(position);
        m_headersRows.append(listDouble[Col_Debtor].toString());
        endInsertRows();
        return true;

}

bool DisplayModel::removeRows(int position, int count, const QModelIndex & parent)
{
        Q_UNUSED(parent);
        beginRemoveRows(parent, position, position+count-1);
        int rows = m_listsOfValuesbyRows->size();
        for (int row=0; row < count; row++) {
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(row) ;
            m_listsOfValuesbyRows -> remove(rows - row -1);
        }
        endRemoveRows();
        return true;
}

bool DisplayModel::submit()
{
        return QAbstractTableModel::submit();
}


QVariant DisplayModel::data(const QModelIndex &index, int role) const
{
        QVariant data;
        if (!index.isValid()) {
            if (WarnDebugMessage)
                qWarning() << __FILE__ << QString::number(__LINE__) << "index not valid" ;
            return QVariant();
        }


        if (role==Qt::EditRole || role==Qt::DisplayRole) {
            int row = index.row();
            const QList<QVariant> & valuesListByRow = m_listsOfValuesbyRows->at(row);
            data = valuesListByRow[index.column()];
        }
        return data;
}

bool DisplayModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
        bool ret = true;
        if (!index.isValid()) {
            if (WarnDebugMessage)
                qWarning() << __FILE__ << QString::number(__LINE__) << "index not valid" ;
            return false;
        }

        if (role==Qt::EditRole) {
            QList<QVariant> list;
            int row = index.row();
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(row) ;
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << value.toString() ;
            list = m_listsOfValuesbyRows->at(row);
            switch(index.column()){
            case Col_Cash :
                list.replace(Col_Cash,value);
                break;
            case Col_Cheque :
                list.replace(Col_Cheque,value);
                break;
            case Col_Visa :
                list.replace(Col_Visa,value);
                break;
            case Col_Banking :
                list.replace(Col_Banking,value);
                break;
            case Col_Other :
                list.replace(Col_Other,value);
                break;
            case Col_Due :
                list.replace(Col_Due,value);
                break;
            case Col_Debtor :
                list.replace(Col_Debtor,value);
                break;
            case Col_Site :
                list.replace(Col_Site,value);
                break;
            case Col_DistRule :
                list.replace(Col_DistRule,value);
                break;
            case Col_Act :
                list.replace(Col_Act,value);
            default :
                break;
            }
            m_listsOfValuesbyRows->replace(row,list);
            Q_EMIT dataChanged(index, index);
            ret = true;
        }
        return ret;
}

QVariant DisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
        if (role==Qt::DisplayRole) {
            if (orientation==Qt::Horizontal) {
                switch (section) {
                case Col_Cash: return m_headersColumns.value(Col_Cash);//"Cash";
                case Col_Cheque: return m_headersColumns.value(Col_Cheque);//"Cheque";
                case Col_Visa: return m_headersColumns.value(Col_Visa);//return "Visa";
                case Col_Banking: return m_headersColumns.value(Col_Banking);//"Banking";
                case Col_Other: return m_headersColumns.value(Col_Other);//"Other";
                case Col_Due: return m_headersColumns.value(Col_Due);//"Due";
                case Col_Debtor : return m_headersColumns.value(Col_Debtor);//"debtor";
                case Col_Site : return m_headersColumns.value(Col_Site);//"site";
                case Col_DistRule : return m_headersColumns.value(Col_DistRule);//"distRule";
                case Col_Act : return m_headersColumns.value(Col_Act);//"act";
                    //return QVariant(m_headersColumns[section]);
                }
            }
            else if (orientation==Qt::Vertical) {
                return QVariant(m_headersRows[section]);
            }
        }

        return QVariant();
}

bool DisplayModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant & value, int role)
{
        if (role == Qt::EditRole||role == Qt::DisplayRole) {
            if (orientation == Qt::Vertical) {
                m_headersRows.insert(section,value.toString());
            }
            else if (orientation == Qt::Horizontal){
                m_headersColumns.insert(section,value.toString());
            }
        }

        else {
            return false;
        }

        Q_EMIT QAbstractTableModel::headerDataChanged(orientation, section, section) ;
        return true;
}

QSqlError DisplayModel::lastError()
{
        return lastError();
}

Qt::ItemFlags DisplayModel::flags(const QModelIndex &index) const
{
        if (   index.column()==Col_Cash
               || index.column()==Col_Cheque
               || index.column()==Col_Visa
               || index.column()==Col_Banking
               || index.column()==Col_Other
               || index.column()==Col_Due
               || index.column()==Col_Debtor
               || index.column()==Col_Site
               || index.column()==Col_DistRule
               || index.column()==Col_Act ) {
            return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
        } else {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
}

//////////////////////////////////////////////////////////////////////////
/////////////treeview/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

treeViewsActions::treeViewsActions(QWidget *parent):QTreeView(parent){
    m_deleteThesaurusValue = new QAction(tr("Delete value"),this);
    m_choosepreferredValue = new QAction(tr("Choose value as preferred"),this);
    m_userUuid = user()->uuid();
    connect(m_choosepreferredValue,SIGNAL(triggered(bool)),this,SLOT(choosepreferredValue(bool)));
    connect(m_deleteThesaurusValue,SIGNAL(triggered(bool)),this,SLOT(deleteBox(bool)));
    connect(user(), SIGNAL(userChanged()), this, SLOT(userIsChanged()));
}

treeViewsActions::~treeViewsActions()
{
    qWarning() << "treeViewsActions::~treeViewsActions()" ;
}

void treeViewsActions::userIsChanged(){
    m_userUuid = user()->uuid();
    if (!fillActionTreeView())
    {
        qWarning()  << __FILE__ << QString::number(__LINE__) << "index is not valid";
    }
}

/*void treeViewsActions::mousePressEvent(QMouseEvent *event){
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " in  tree clicked" ;
    if(event->button() == Qt::RightButton){
        if(isChildOfThesaurus()){
            blockSignals(true);
            if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " in treeview right button " ;
            m_menuRightClic = new QMenu(this);
            m_menuRightClic -> addAction(m_choosepreferredValue);
            m_menuRightClic-> addAction(m_deleteThesaurusValue);
            m_menuRightClic->exec(event->globalPos());
            blockSignals(false);
        }

    }
    if(event->button() == Qt::LeftButton){
            if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " in left press button " ;
            blockSignals(false);
            QTreeView::mousePressEvent(event);
    }
}*/

void treeViewsActions::mouseReleaseEvent(QMouseEvent *event){
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " in  tree clicked" ;
    if(event->button() == Qt::RightButton){
        if(isChildOfThesaurus()){
            blockSignals(true);
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " in treeview release right button " ;
            m_menuRightClic = new QMenu(this);
            m_menuRightClic -> addAction(m_choosepreferredValue);
            m_menuRightClic-> addAction(m_deleteThesaurusValue);
            m_menuRightClic->exec(event->globalPos());
            blockSignals(false);
        }

    }
    if(event->button() == Qt::LeftButton) {
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " in left button " ;
        blockSignals(false);
        QTreeView::mouseReleaseEvent(event);

    }
}

void treeViewsActions::deleteBox(bool b){
    Q_UNUSED(b);
    bool yes = Utils::yesNoMessageBox(tr("Delete :"),
                                      tr("Do you want to delete selected item?"));
    if (yes) {
        QModelIndex index = currentIndex();
        deleteItemFromThesaurus(index);
    }
}

void treeViewsActions::choosepreferredValue(bool b){
    Q_UNUSED(b);
    bool yes = Utils::yesNoMessageBox(tr("Preferred value : "),
                                      tr("Do you want to set this item as preferred value?"));
    if (yes) {
        QModelIndex index = currentIndex();
        if (!addpreferredItem(index)) {
            Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Unable to set this item as the preferred one."));
        }
    }
}

bool treeViewsActions::addpreferredItem(QModelIndex &index){
    bool ret = true;
    QString data = index.data().toString();
    receiptsEngine r;
    if (!r.addBoolTrue(data)) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Cannot change boolean value in thesaurus: %1").arg(data));
        ret = false;
    }
    fillActionTreeView();
    return ret;
}

bool treeViewsActions::isChildOfThesaurus() {
    bool ret = false;
    QModelIndex current = currentIndex();
    QModelIndex indexParent = treeModel()->parent(current);
    QString dataParent = treeModel()->data(indexParent).toString();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " dataParent =" << dataParent ;
    if (indexParent.row()==THESAURUS_ITEM) {
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " isChildOfThesaurus " ;
        ret = true;
    }
    return ret;
}

bool treeViewsActions::fillActionTreeView()
{
    bool b = true;
    ReceiptsTools rt;
    m_actionsTreeModel = new QStandardItemModel;
    QStringList listOfMainActions;
    QMap<int,QString> parametersMap;

    parametersMap.insert(THESAURUS_ITEM,rt.getStringFromRows(THESAURUS_ITEM));
    parametersMap.insert(ALL_VALUES_ITEM,rt.getStringFromRows(ALL_VALUES_ITEM));
    parametersMap.insert(PREFERED_VALUE_ITEM,rt.getStringFromRows(PREFERED_VALUE_ITEM));
    parametersMap.insert(ROUND_TRIP_ITEM,rt.getStringFromRows(ROUND_TRIP_ITEM));
    parametersMap.insert(FREE_VALUE_ITEM,rt.getStringFromRows(FREE_VALUE_ITEM));

    listOfMainActions = parametersMap.values();
    //insert items from tables if available
    //QMap<QString,QString> m_mapSubItems;
    m_mapSubItems.clear();
    ReceiptsManager manager;
    QList<int> listOfEnums;
    listOfEnums << THESAURUS_ITEM << ALL_VALUES_ITEM << PREFERED_VALUE_ITEM
                << ROUND_TRIP_ITEM << FREE_VALUE_ITEM ;
    //QString strKeysParameters;
    for (int item = 0; item < listOfEnums.size(); ++item)
    {
        QString table = parametersMap.value(item);
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << "table" << table;
        QStringList listOfItemsOfTable;
        listOfItemsOfTable = manager.getParametersData(m_userUuid,item).keys();//QHash<QString,QVariant> name,uid
        QString strItemsOfTable;
        foreach(strItemsOfTable,listOfItemsOfTable){
            m_mapSubItems.insertMulti(table,strItemsOfTable);
        }
        //default values if unavailables :
        if (listOfItemsOfTable.size()<1) {
            if (WarnDebugMessage) {
                qDebug() << __FILE__ << QString::number(__LINE__) << "listOfItemsOfTable.size()<1"  ;
                qDebug() << __FILE__ << QString::number(__LINE__) << "table  =" << table ;
            }
            if (item == THESAURUS_ITEM) {
                if (WarnDebugMessage)
                    qDebug() << __FILE__ << QString::number(__LINE__) << " in thesaurus " ;
                m_mapSubItems.insertMulti(table,"CS");
                m_mapSubItems.insertMulti(table,"V");
            }
            else {
                if (WarnDebugMessage)
                    qWarning() << __FILE__ << QString::number(__LINE__)
                               << " No default value for "<< table ;
            }
        }
    }


    QStandardItem *parentItem = treeModel()->invisibleRootItem();
    if (!parentItem->index().isValid()){
        if (WarnDebugMessage)
            qWarning() << __FILE__ << QString::number(__LINE__) << "parentItem is not valid";
    }
    QString strMainActions;
    /*QMap<int,QStandardItem*>*/ m_mapOfMainItems.clear();
    foreach(strMainActions,listOfMainActions) {
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " strMainActions =" << strMainActions ;
        QStandardItem *actionItem = new QStandardItem(strMainActions);
        actionItem->setEditable(false);
        actionItem->setEnabled(true);
        int row = 0;
        int width = 100;
        //treeViewsActions and colors
        width = actionItem->sizeHint().width();
        QFont itemsFont("Times", 14, QFont::Bold);
        actionItem->setSizeHint(QSize(width,30));
        actionItem->setFont(itemsFont);
        if (strMainActions == rt.getStringFromRows(PREFERED_VALUE_ITEM)) {
            QBrush red(Qt::red);
            actionItem->setForeground(red);
            m_mapOfMainItems.insert(PREFERED_VALUE_ITEM,actionItem);
            row = PREFERED_VALUE_ITEM;
        } else if (strMainActions == rt.getStringFromRows(THESAURUS_ITEM)) {
            QBrush red(Qt::red);
            actionItem->setForeground(red);
            m_mapOfMainItems.insert(THESAURUS_ITEM,actionItem);
            row = THESAURUS_ITEM;
        } else if (strMainActions == rt.getStringFromRows(ALL_VALUES_ITEM)) {
            QBrush blue(Qt::blue);
            actionItem->setForeground(blue);
            m_mapOfMainItems.insert(ALL_VALUES_ITEM,actionItem);
            row = ALL_VALUES_ITEM;
        } else if (strMainActions == rt.getStringFromRows(ROUND_TRIP_ITEM)) {
            QBrush blue(Qt::blue);
            actionItem->setForeground(blue);
            m_mapOfMainItems.insert(ROUND_TRIP_ITEM,actionItem);
            row = ROUND_TRIP_ITEM;
        } else if (strMainActions == rt.getStringFromRows(FREE_VALUE_ITEM))
        {
            QColor greenColor(54,147,0,255);
            QBrush green(greenColor);
            actionItem->setForeground(green);
            m_mapOfMainItems.insert(FREE_VALUE_ITEM,actionItem);
            row = FREE_VALUE_ITEM;
        }


        else {
            if (WarnDebugMessage)
                qWarning() << __FILE__ << QString::number(__LINE__) << "Error color treeViewsActions." ;
        }

        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << QString::number(row);

    }//

    for (int i = 0; i < listOfEnums.size(); ++i)
    {
        QStandardItem *actionItem = m_mapOfMainItems.value(i);
        actionItem->setEditable(false);
        treeModel()->insertRow(i,actionItem);
        QStringList listSubActions;
        listSubActions = m_mapSubItems.values(actionItem->text());
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << "listSubActions" << QString::number(listSubActions.size())
                     << " " << actionItem->text();
        QString strSubActions;
        foreach(strSubActions,listSubActions){
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " strSubActions =" <<
                            strSubActions ;
            QStandardItem *subActionItem = new QStandardItem(strSubActions);
            subActionItem->setEditable(false);
            actionItem->appendRow(subActionItem);
            if (!subActionItem->index().isValid()) {
                if (WarnDebugMessage)
                    qWarning() << __FILE__ << QString::number(__LINE__) << "subActionItem is not valid";
                b = false;
            }
        }
    }//end of  actionItem

    setHeaderHidden(true);
    setSortingEnabled ( false );
    setStyleSheet("background-color: rgb(201, 201, 201)");
    // actionsTreeView->setStyleSheet("foreground-color: red");
    setModel(treeModel());
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << "ACTION TREEVIEW FILLED UP"  ;
    return b;
}

bool treeViewsActions::deleteItemFromThesaurus(QModelIndex &index){
    bool ret = true;
    QString data = index.data().toString();
    const QString userUid = user()->uuid();
    receiptsEngine r;
    if (!r.deleteFromThesaurus(data,userUid)) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Cannot delete in thesaurus:")+data);
        ret = false;
    }
    fillActionTreeView();
    return ret;
}

void treeViewsActions::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    if (e->type()==QEvent::LanguageChange) {
        delete m_actionsTreeModel;
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " langage changed " ;
        if (!fillActionTreeView()) {
            if (WarnDebugMessage)
                qWarning() << __FILE__ << QString::number(__LINE__) << "index is not valid";
        }
        reset();
    }
}

///////////////////////////////////////////////////////////////
///LISTVIEW
///////////////////////////////////////////////////////////////

ChosenListView::ChosenListView(QObject * parent, InternalAmount::DisplayModel *amountModel) {
    setObjectName("ChosenListView");
    m_parent = parent;
    m_deleteInReturnedList = new QAction(tr("Delete this item"),parent);
    m_clear = new QAction(tr("Clear all."),parent);
    //m_deleteInReturnedList->setCheckable(true);
    m_deleteInReturnedList->setEnabled(true);
    m_clear->setEnabled(true);
    //m_deleteInReturnedList->setCheckable(true);
    //m_clear->setCheckable(true);
    m_amountModel = amountModel;
    connect(m_clear,SIGNAL(triggered(bool)),parent,SLOT(clearAll(bool)));
    connect(m_deleteInReturnedList,SIGNAL(triggered(bool)),this,SLOT(deleteItem(bool)));
}

ChosenListView::~ChosenListView(){
    qWarning() << "ChosenListView::~ChosenListView()" ;
}

void ChosenListView::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    if (e->type()==QEvent::LanguageChange) {
        m_deleteInReturnedList = new QAction(tr("Delete this item"),m_parent);
        m_clear = new QAction(tr("Clear all."),m_parent);
        m_deleteInReturnedList->setEnabled(true);
        m_clear->setEnabled(true);
        //m_deleteInReturnedList->setCheckable(true);
        //m_clear->setCheckable(true);
        connect(m_clear,SIGNAL(triggered(bool)),m_parent,SLOT(clearAll(bool)));
        connect(m_deleteInReturnedList,SIGNAL(triggered(bool)),this,SLOT(deleteItem(bool)));
        reset();
    }
}

void ChosenListView::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton){
        blockSignals(true);
        m_menu  = new QMenu(this);
        m_menu  -> addAction(m_clear);
        m_menu  -> addAction (m_deleteInReturnedList);
        m_menu  ->exec(event->globalPos());
        blockSignals(false);
    }
    else
    {
        blockSignals(false);
        QListView::mouseReleaseEvent(event);
    }
}

void ChosenListView::deleteItem(bool b)
{
    Q_UNUSED(b);
    QModelIndex index = currentIndex();
    int row = index.row();
    model()->removeRows(row,1,QModelIndex());
    m_amountModel->removeRows(row,1,QModelIndex());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////RECEIPTS VIEWER/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ReceiptViewer::ReceiptViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReceiptViewer)
{
    if (user())
        m_userUuid = user()->uuid();
    /*receiptsEngine re;
    re.hideNamesOfPatients();*/
    ReceiptsManager rManager;
    m_kilometers = 0.00 ;
    m_distanceRuleValue = 0.00;
    m_insuranceUid = 0;
    m_model = new InternalAmount::DisplayModel(this);
    ui->setupUi(this);
    ui->dateExecution->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    ui->datePayment->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    ui->saveAndQuitButton->hide();
    ui->quitButton->hide();
    ui->nameLabel->hide();
    ui->nameEdit->hide();
    if (rManager.isMedintuxArg())
    {
        ui->saveAndQuitButton->show();
        ui->quitButton->show();
        setAttribute(Qt::WA_DeleteOnClose);
    }
    if (rManager.isFreaccountIndependant())
    {
        ui->nameLabel->show();
        ui->nameEdit->show();
    }
    if (WarnDebugMessage)
        qWarning() << __FILE__ << QString::number(__LINE__) << "ReceiptViewer" ;
    ui->amountsView->setShowGrid(false);
    /*ui->amountsView->verticalHeader()->setResizeMode(QHeaderView::Interactive);
    ui->amountsView->verticalHeader()->setDefaultSectionSize(10);
    ui->amountsView->verticalHeader()->setDefaultAlignment(Qt::AlignTop);*/
    ui->amountsView->verticalHeader()->setStretchLastSection ( false );

#if QT_VERSION < 0x050000
    //ui->amountsView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->amountsView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->amountsView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
#else
    // Qt5
    //ui->amountsView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->amountsView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->amountsView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
#endif

    ui->amountsView->horizontalHeader()->setMinimumSectionSize(100);
    //ui->amountsView->horizontalHeader()->setCascadingSectionResizes (true);
    //ui->amountsView->horizontalHeader()->setStretchLastSection ( true );
    m_model->setHeaderData(int(HDCash),Qt::Horizontal,tr("Cash"));
    m_model->setHeaderData(HDCheck,Qt::Horizontal,tr("Check"));
    m_model->setHeaderData(HDVisa,Qt::Horizontal,tr("Visa"));
    m_model->setHeaderData(HDBanking,Qt::Horizontal,tr("Banking"));
    m_model->setHeaderData(HDOther,Qt::Horizontal,tr("Other"));
    m_model->setHeaderData(HDDue,Qt::Horizontal,tr("Due"));
    m_model->setHeaderData(HDAct,Qt::Horizontal,tr("Act"));


    ui->amountsView->setModel(m_model);
    ui->amountsView->setItemDelegateForColumn(Cash, new Utils::SpinBoxDelegate(this,0.00,100000.00,true));
    ui->amountsView->setItemDelegateForColumn(Check, new Utils::SpinBoxDelegate(this,0.00,100000.00,true));
    ui->amountsView->setItemDelegateForColumn(Visa, new Utils::SpinBoxDelegate(this,0.00,100000.00,true));
    ui->amountsView->setItemDelegateForColumn(Banking, new Utils::SpinBoxDelegate(this,0.00,100000.00,true));
    ui->amountsView->setItemDelegateForColumn(Other, new Utils::SpinBoxDelegate(this,0.00,100000.00,true));
    ui->amountsView->setItemDelegateForColumn(Due, new Utils::SpinBoxDelegate(this,0.00,100000.00,true));
    ui->amountsView->setColumnHidden(InternalAmount::DisplayModel::Col_Debtor,true);
    ui->amountsView->setColumnHidden(InternalAmount::DisplayModel::Col_Site,true);
    ui->amountsView->setColumnHidden(InternalAmount::DisplayModel::Col_DistRule,true);

    ui->amountsView->resizeRowsToContents();
    ui->amountsView->resizeColumnsToContents();

    ui->dateExecution->setDisplayFormat("yyyy-MM-dd");
    ui->dateExecution->setDate(QDate::currentDate());
    ui->datePayment->setDisplayFormat("yyyy-MM-dd");
    ui->datePayment->setDate(QDate::currentDate());

    ui->datePayment->hide();
    ui->paymentLabel->hide();

    /*ui->dateBanked->setDisplayFormat("yyyy-MM-dd");
    ui->dateBanked->setDate(QDate::currentDate());
    ui->dateBook->setDisplayFormat("yyyy-MM-dd");
    ui->dateBook->setDate(QDate::currentDate());*/

    //icons and shortcuts
    ui->saveButton->setShortcut(QKeySequence::InsertParagraphSeparator);
    ui->saveButton->setIcon(theme()->icon(Core::Constants::ICONSAVE));
    ui->saveButton->setToolTip(QKeySequence(QKeySequence::InsertParagraphSeparator).toString());
    ui->quitButton->setShortcut(QKeySequence::Close);
    ui->quitButton->setIcon(theme()->icon(Core::Constants::ICONQUIT));
    ui->quitButton->setToolTip(QKeySequence(QKeySequence::Close).toString());
    ui->thesaurusButton->setShortcut(QKeySequence("Ctrl+t"));
    ui->thesaurusButton->setToolTip(QKeySequence(QKeySequence("Ctrl+t")).toString());
    ui->thesaurusButton->setIcon(theme()->icon("bookmark_add.png"));
    ui->goToControlButton->setIcon(theme()->icon(Core::Constants::ICONNEXT));
    ui->goToControlButton->setShortcut(QKeySequence::Forward);
    ui->goToControlButton->setToolTip(QKeySequence(QKeySequence::Forward).toString());

    m_actionTreeView = new treeViewsActions(this);
    m_vbox = new QVBoxLayout;
    m_vbox->addWidget(m_actionTreeView);
    ui->actionsBox->setLayout(m_vbox);
    if (!m_actionTreeView->fillActionTreeView()) {
        if (WarnDebugMessage)
            qWarning() << __FILE__ << QString::number(__LINE__) << "index is not valid";
    }
    m_returnedListView = new ChosenListView(this,m_model);
    m_returnedListView->setStyleSheet("background-color: rgb(201, 201, 201)");
    m_vboxForList = new QVBoxLayout;
    m_vboxForList->addWidget(m_returnedListView);
    ui->chosenValuesBox->setLayout(m_vboxForList);
    m_modelReturnedList = new QStringListModel;
    m_returnedListView->setModel(m_modelReturnedList);
    m_returnedListView->setEnabled(true);
    m_returnedListView->show();
    //preferential choices in the tree view.
    QString site = QString("Sites");
    QString distRule = QString("Distance rules");
    QString debtor = QString("Debtor");
    m_siteUid = firstItemchosenAsPreferential(site);
    m_distanceRuleValue = firstItemchosenAsPreferential(distRule).toDouble();
    m_distanceRuleType = rManager.getpreferredDistanceRule().toString();
    m_insuranceUid = firstItemchosenAsPreferential(debtor);
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__)
                 << " site,dist,ins preferred =" << m_siteUid.toString()
                 << QString::number(m_distanceRuleValue)
                 << m_insuranceUid.toString() ;

    //right click

    //ui_controlreceipts
    m_control = new ControlReceipts(this);
    m_control->hide();
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(quitFreeAccount()));
    connect(ui->saveButton,SIGNAL(pressed()),this,SLOT(save()));
    connect(ui->saveAndQuitButton,SIGNAL(pressed()),this,SLOT(saveAndQuit()));
    connect(ui->thesaurusButton,SIGNAL(pressed()),this,SLOT(saveInThesaurus()));
    connect(ui->goToControlButton,SIGNAL(pressed()),this,SLOT(showControlReceipts()));
    if (!connect(m_actionTreeView,SIGNAL(clicked(const QModelIndex&)),this,SLOT(actionsOfTreeView(const QModelIndex&))))
    {
        if (WarnDebugMessage)
            qWarning() << __FILE__ << QString::number(__LINE__) << "unable to connect m_actionTreeView";
    }


    connect(m_control,SIGNAL(isClosing()),this,SLOT(controlReceiptsDestroyed()));
    connect(user(), SIGNAL(userChanged()), this, SLOT(userUid()));
    //connect(m_returnedListView,SIGNAL(pressed(QModelIndex&)),this,SLOT(deleteItem(QModelIndex&)));

}

ReceiptViewer::~ReceiptViewer()
{
    qWarning() << "ReceiptViewer::~ReceiptViewer()" ;
    /*delete m_returnedListView;
    delete m_actionTreeView;
    delete ui;*/
}

void ReceiptViewer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        delete m_actionTreeView;
        delete m_vbox;
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " in  ReceiptViewer::changeEvent(QEvent *e)"  ;
        //treeViewsActions
        m_actionTreeView = new treeViewsActions(this);
        m_vbox = new QVBoxLayout;
        m_vbox->addWidget(m_actionTreeView);
        ui->actionsBox->setLayout(m_vbox);
        if (!m_actionTreeView->fillActionTreeView()) {
            if (WarnDebugMessage)
                qWarning()  << __FILE__ << QString::number(__LINE__) << "index is not valid";
        }
        if (!connect(m_actionTreeView,SIGNAL(clicked(const QModelIndex&)),this,SLOT(actionsOfTreeView(const QModelIndex&)))) {
            if (WarnDebugMessage)
                qWarning() << __FILE__ << QString::number(__LINE__) << "unable to connect m_actionTreeView";
        }
        //amountsView
        m_model->setHeaderData(int(HDCash),Qt::Horizontal,tr("Cash"));
        m_model->setHeaderData(HDCheck,Qt::Horizontal,tr("Check"));
        m_model->setHeaderData(HDVisa,Qt::Horizontal,tr("Visa"));
        m_model->setHeaderData(HDBanking,Qt::Horizontal,tr("Banking"));
        m_model->setHeaderData(HDOther,Qt::Horizontal,tr("Other"));
        m_model->setHeaderData(HDDue,Qt::Horizontal,tr("Due"));
        m_model->setHeaderData(HDAct,Qt::Horizontal,tr("Act"));
        break;
    default:
        break;
    }
}

void ReceiptViewer::setPosition(QWidget *parent){
    QRect rect = parent->rect();
    setGeometry(parent->x(),parent->y(),rect.width()-10,rect.height()-10);
}


void ReceiptViewer::actionsOfTreeView(const QModelIndex & index) {
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << "in actionsOfTreeView(const QModelIndex& index) ";
    QString data = index.data(Qt::DisplayRole).toString();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " DATA =" << data;
    ReceiptsTools rt;
    ReceiptsManager manager;
    QHash<QString,QString> hashOfValues;
    int typeOfPayment = ReceiptsConstants::Cash;
    double percentage = 100.00;
    QVariant  debtor;
    QVariant site;
    QVariant distrules;
    if(index.row() == ALL_VALUES_ITEM && index.parent() == QModelIndex() ){ //values
        findReceiptsValues *rv = new findReceiptsValues(this);
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " in findReceiptsValues AND VALUES "  ;
        if(rv->exec() == QDialog::Accepted) {
            hashOfValues = rv -> getchosenValues();
            ChoiceDialog choice(rv,false,false);
            if(hashOfValues.keys().size() > 0){
                if(choice.exec() == QDialog::Accepted){
                    QStandardItemModel * model = choice.getChoicePercentageDebtorSiteDistruleModel();
                    for (int i = 0; i < model->rowCount(QModelIndex()); ++i) {
                        typeOfPayment = model->data(model->index(i,choice.TYPE_OF_CHOICE),Qt::DisplayRole).toInt();
                        percentage = model->data(model->index(i,choice.PERCENTAGE),Qt::DisplayRole).toDouble();
                        debtor = model->data(model->index(i,choice.DEBTOR),Qt::DisplayRole);
                        site = model->data(model->index(i,choice.SITE),Qt::DisplayRole);
                        distrules = model->data(model->index(i,choice.DISTRULES),Qt::DisplayRole);
                        /*QStringList*/ m_listOfValues << hashOfValues.keys();
                        //m_listOfValues.removeDuplicates();
                        if (WarnDebugMessage)
                            LOG_ERROR("percentages = "+QString::number(percentage));
                        m_modelReturnedList->setStringList(m_listOfValues);
                        fillModel(hashOfValues,typeOfPayment,percentage,debtor,site,distrules,i);
                    }
                    delete model;
                }
            }
        }
    }
    if(index.row() == PREFERED_VALUE_ITEM && index.parent() == QModelIndex()){// preferential act of payment
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " PREFERENTIAL_VALUE";
        ChoiceDialog choice(this,false,false);
        if(choice.exec() == QDialog::Accepted){
            QStandardItemModel * model = choice.getChoicePercentageDebtorSiteDistruleModel();
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " model->rowCount() =" << QString::number(model->rowCount()) ;

            for (int i = 0; i < model->rowCount(); ++i) {
                QHash<QString,QString> hashOfValues;
                const QString userUuid = m_userUuid;
                typeOfPayment = model->data(model->index(i,choice.TYPE_OF_CHOICE),Qt::DisplayRole).toInt();
                if (WarnDebugMessage)
                    qDebug() << __FILE__ << QString::number(__LINE__) << " typeOfPayment =" << QString::number(typeOfPayment) ;

                percentage = model->data(model->index(i,choice.PERCENTAGE),Qt::DisplayRole).toDouble();
                debtor = model->data(model->index(i,choice.DEBTOR),Qt::DisplayRole);
                if (WarnDebugMessage)
                    qDebug() << __FILE__ << QString::number(__LINE__) << " debtor =" << debtor.toString() ;
                site = model->data(model->index(i,choice.SITE),Qt::DisplayRole);
                distrules = model->data(model->index(i,choice.DISTRULES),Qt::DisplayRole);
                if (WarnDebugMessage)
                    qDebug() << __FILE__ << QString::number(__LINE__) << " preferred value =" << data ;
                hashOfValues = manager.getPreferentialActFromThesaurus(userUuid);
                QString preferredAct = hashOfValues.keys()[0] ;
                if (WarnDebugMessage)
                    qDebug() << __FILE__ << QString::number(__LINE__) << " preferential acts =" << preferredAct;
                if (preferredAct == "NULL")
                {
                    qWarning() << __FILE__ << QString::number(__LINE__) << "preferredAct == NULL" ;
                    return;
                }
                if (hashOfValues.size() < 1)  {
                    hashOfValues.insertMulti("NULL","0");//preferential act
                    Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("You have to insert your preferred "
                                                                               "value\nin thesaurus\nand choose it as preferred."));
                }
                m_listOfValues << hashOfValues.keys();
                //m_listOfValues.removeDuplicates();
                m_modelReturnedList->setStringList(m_listOfValues);
                fillModel(hashOfValues,typeOfPayment,percentage,debtor,site,distrules,i);
            }
            delete model;
        }
    }

    /* if (manager.getDistanceRules().keys().contains(rt.getStringFromRows(ROUND_TRIP_ITEM)))
    {
          if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " in getDistanceRules";
          m_distanceRuleValue = manager.getDistanceRules().value(data).toDouble();
          m_distanceRuleType = data;
          if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " m_distanceRuleValue =" << QString::number(m_distanceRuleValue) ;
          if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " m_distanceRuleType =" << m_distanceRuleType ;
        }*/
    if (index.row() == ROUND_TRIP_ITEM && index.parent() == QModelIndex())
    {
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " in ROUND_TRIP";
        ChoiceDialog dist(this,true,false);
        if (dist.exec()== QDialog::Accepted)
        {
            QStandardItemModel * model = dist.getChoicePercentageDebtorSiteDistruleModel();
            for (int i = 0; i < model->rowCount(); i += 1)
            {
                QHash<QString,QString> hashOfValues;
                m_kilometers = dist.getDistanceNumber(m_distanceRuleType);
                double value = m_kilometers *m_distanceRuleValue;
                if (WarnDebugMessage)
                    qDebug() << __FILE__ << QString::number(__LINE__) << " m_distanceRuleValue =" << QString::number(m_distanceRuleValue) ;
                if (m_distanceRuleValue == 0.00) {
                    qWarning() << __FILE__ << QString::number(__LINE__) << "no m_distanceRuleValue available" ;
                    QString information = tr("There are no distance rule value available.\n"
                                             "You can create one in Preferences.");
                    Utils::warningMessageBox(tr("No distance rule."), information);
                    return;
                }
                typeOfPayment = model->data(model->index(i,dist.TYPE_OF_CHOICE),Qt::DisplayRole).toInt();
                percentage = model->data(model->index(i,dist.PERCENTAGE),Qt::DisplayRole).toDouble();
                debtor = model->data(model->index(i,dist.DEBTOR),Qt::DisplayRole);
                site = model->data(model->index(i,dist.SITE),Qt::DisplayRole);
                distrules = model->data(model->index(i,dist.DISTRULES),Qt::DisplayRole);;
                hashOfValues.insertMulti("DistPrice",QString::number(value));
                if (WarnDebugMessage)
                    qDebug() << __FILE__ << QString::number(__LINE__) << " distance =" << QString::number(m_kilometers) ;
                if (WarnDebugMessage)
                    qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << QString::number(value) ;
                m_listOfValues << tr("Kilometers");
                //m_listOfValues.removeDuplicates();
                m_modelReturnedList->setStringList(m_listOfValues);
                fillModel(hashOfValues,typeOfPayment,percentage,debtor,site,distrules,i);
            }
            delete model;
        }

    }
    if (index.parent().row()==THESAURUS_ITEM)
    {
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " IN THESAURUS " ;
        ChoiceDialog choice(this,false,false);
        if(choice.exec() == QDialog::Accepted){
            QStandardItemModel * model = choice.getChoicePercentageDebtorSiteDistruleModel();
            for (int i = 0; i < model->rowCount(); i += 1)
            {
                QHash<QString,QString> hashOfValues;
                if (WarnDebugMessage)
                    qDebug() << __FILE__ << QString::number(__LINE__) << " IN THESAURUS FOR "  ;
                typeOfPayment = model->data(model->index(i,choice.TYPE_OF_CHOICE),Qt::DisplayRole).toInt();
                percentage = model->data(model->index(i,choice.PERCENTAGE),Qt::DisplayRole).toDouble();
                debtor = model->data(model->index(i,choice.DEBTOR),Qt::DisplayRole);
                site = model->data(model->index(i,choice.SITE),Qt::DisplayRole);
                distrules = model->data(model->index(i,choice.DISTRULES),Qt::DisplayRole);
                QStringList list;
                list = data.split("+");
                receiptsEngine r;
                QHash<QString,double> hashFromMp ;
                const QString field = "NAME";
                QString str;
                foreach(str,list){
                    if (WarnDebugMessage)
                        qDebug() << __FILE__ << QString::number(__LINE__) << " str =" << str ;
                    if (str .contains( tr("thesaurus")))
                    {
                        qWarning() << __FILE__ << QString::number(__LINE__) << "no thesaurus value available" ;
                        return;
                    }
                    QStringList modifDataList = str.split("*");
                    QString modifier;
                    if (modifDataList.size() < 2)
                    {
                        modifier = "1.0";
                    }
                    else{
                        modifier = modifDataList[1];
                    }
                    double modifDouble = modifier.toDouble();
                    QString data = modifDataList[0];
                    hashFromMp = r.getFilteredValueFromMedicalProcedure(data,field);
                    if (WarnDebugMessage)
                        qDebug() << __FILE__ << QString::number(__LINE__) << " hashFromMp.size() " << QString::number(hashFromMp.size());
                    QString value = QString::number(hashFromMp.value(data));
                    if (WarnDebugMessage)
                        qDebug() << __FILE__ << QString::number(__LINE__) << value;
                    if (WarnDebugMessage)
                        qDebug() << __FILE__ << QString::number(__LINE__) << QString::number(percentage);
                    if (value.toDouble() == 0.0)
                    {
                        if (value.contains(","))
                        {
                            value.replace(",",QLocale::c().decimalPoint ());
                        }
                        else if (value.contains("."))
                        {
                            value.replace(".",QLocale::c().decimalPoint ());
                        }
                    }
                    double valueDouble = value.toDouble();
                    double valueModified = valueDouble * modifDouble;
                    hashOfValues.insertMulti(str,QString::number(valueModified));
                    if (WarnDebugMessage)
                        qDebug() << __FILE__ << QString::number(__LINE__) << " hashOfValues.size() in getHashOfThesaurus " << QString::number(hashOfValues.size());
                    m_listOfValues << str;
                }
                m_listOfValues.removeDuplicates();
                m_modelReturnedList->setStringList(m_listOfValues);
                fillModel(hashOfValues,typeOfPayment,percentage,debtor,site,distrules,i);
            }
            delete model;
        }
    }
    if (index.row() == FREE_VALUE_ITEM && index.parent() == QModelIndex())
    {
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " PREFERENTIAL_VALUE";
        ChoiceDialog choice(this,false,true);
        if(choice.exec() == QDialog::Accepted){
            QStandardItemModel * model = choice.getChoicePercentageDebtorSiteDistruleModel();
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " model->rowCount() =" << QString::number(model->rowCount()) ;

            for (int i = 0; i < model->rowCount(); ++i) {
                QHash<QString,QString> hashOfValues;
                const QString userUuid = m_userUuid;
                m_freePayer = QString();
                m_freePayer = choice.getFreePayer();
                typeOfPayment = model->data(model->index(i,choice.TYPE_OF_CHOICE),Qt::DisplayRole).toInt();

                if (WarnDebugMessage)
                    qDebug() << __FILE__ << QString::number(__LINE__) << " typeOfPayment =" << QString::number(typeOfPayment) ;

                percentage = model->data(model->index(i,choice.PERCENTAGE),Qt::DisplayRole).toDouble();
               	debtor = model->data(model->index(i,choice.DEBTOR),Qt::DisplayRole);

                if (WarnDebugMessage)
                    qDebug() << __FILE__ << QString::number(__LINE__) << " debtor =" << debtor.toString() ;
                site = model->data(model->index(i,choice.SITE),Qt::DisplayRole);
                distrules = model->data(model->index(i,choice.DISTRULES),Qt::DisplayRole);
                QString freeText = choice.getFreeText();
                hashOfValues.insertMulti(freeText,choice.getFreeValue());
                m_listOfValues << hashOfValues.keys();
                //m_listOfValues.removeDuplicates();
                m_modelReturnedList->setStringList(m_listOfValues);
                fillModel(hashOfValues,typeOfPayment,percentage,debtor,site,distrules,i);
            }
            delete model;
        }
    }

}

void ReceiptViewer::fillModel(QHash<QString,QString> &hashOfValues,
                              int typeOfPayment,
                              double percentage,
                              const QVariant & debtor,
                              const QVariant & site,
                              const QVariant & distrules,
                              const int row){
    Q_UNUSED(row);
    int rowOfAmountModel = 0;
    double value = 0.00;
    QHashIterator<QString,QString> it(hashOfValues);
    while(it.hasNext())
    {
        it.next();
        rowOfAmountModel = m_model->rowCount(QModelIndex());
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << "m_model->rowCount()  ="
                     << QString::number(rowOfAmountModel) ;
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " data =" << it.key() ;
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " amount =" << it.value() ;
        QString act = it.key();
        QString valueStr = it.value();
        if (valueStr.toDouble() == 0.0)
        {
            qWarning() << __FILE__ << QString::number(__LINE__) << "value null" ;
            if (valueStr.contains(","))
            {
                if (WarnDebugMessage)
                    qDebug() << __FILE__ << QString::number(__LINE__) << " in , "  ;
                valueStr.replace(",",QLocale::c().decimalPoint ());
            }
            else if (valueStr.contains("."))
            {
                valueStr.replace(".",QLocale::c().decimalPoint ());
            }
        }
        value = valueStr.toDouble();
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << QString::number(value) ;
        value = value*percentage/100.00;
        if (!m_model->insertRows(row,1,QModelIndex()))
        {
            qWarning() << __FILE__ << QString::number(__LINE__)
                       << "unable to insert row = "+QString::number(row) ;
        }
        QModelIndex indexValue = m_model->index(rowOfAmountModel, typeOfPayment);
        QModelIndex indexDebtor = m_model->index(rowOfAmountModel, InternalAmount::DisplayModel::Col_Debtor);
        QModelIndex indexSite = m_model->index(rowOfAmountModel, InternalAmount::DisplayModel::Col_Site);
        QModelIndex indexDistrules = m_model->index(rowOfAmountModel, InternalAmount::DisplayModel::Col_DistRule);
        QModelIndex indexAct = m_model->index(rowOfAmountModel, InternalAmount::DisplayModel::Col_Act);
        //header vertical is debtor
        m_model->setHeaderData(rowOfAmountModel,Qt::Vertical,debtor,Qt::EditRole);
        if (!m_model->setData(indexValue, value, Qt::EditRole))
        {
            qWarning() << __FILE__ << QString::number(__LINE__) << "unable to setData" ;
        }
        if (!m_model->setData(indexDebtor, debtor, Qt::EditRole))
        {
            qWarning() << __FILE__ << QString::number(__LINE__) << "unable to setData" ;
        }
        if (!m_model->setData(indexSite, site, Qt::EditRole))
        {
            qWarning() << __FILE__ << QString::number(__LINE__) << "unable to setData" ;
        }
        if (!m_model->setData(indexDistrules, distrules , Qt::EditRole))
        {
            qWarning() << __FILE__ << QString::number(__LINE__) << "unable to setData" ;
        }
        if (!m_model->setData(indexAct, act , Qt::EditRole))
        {
            qWarning() << __FILE__ << QString::number(__LINE__) << "unable to setData" ;
        }
        m_model->submit();
    }//while
    m_model->submit();
}

void ReceiptViewer::save()
{
    using namespace ::Internal;
    receiptsEngine rIO;
    ReceiptsManager manager;
    QString userUuid = user()->uuid();
    QString textOfListOfActs = m_listOfValues.join("+");
    if (m_model->rowCount(QModelIndex()) < 1)
    {
        Utils::warningMessageBox(tr("No value available !"),
                                 tr("Please choose a value first"));
        return;
    }
    for (int row = 0; row < m_model->rowCount(QModelIndex()); row += 1)
    {
        double cash = m_model->data(m_model->index(row,InternalAmount::DisplayModel::Col_Cash)).toDouble();
        double cheque = m_model->data(m_model->index(row,InternalAmount::DisplayModel::Col_Cheque)).toDouble();
        double visa = m_model->data(m_model->index(row,InternalAmount::DisplayModel::Col_Visa)).toDouble();
        double banking = m_model->data(m_model->index(row,InternalAmount::DisplayModel::Col_Banking)).toDouble();
        double other = m_model->data(m_model->index(row,InternalAmount::DisplayModel::Col_Other)).toDouble();
        double due = m_model->data(m_model->index(row,InternalAmount::DisplayModel::Col_Due)).toDouble();
        QVariant insurance = m_model->data(m_model->index(row,InternalAmount::DisplayModel::Col_Debtor));
        QVariant insuranceUid = rIO.getInsuranceUidFromInsurance(insurance.toString());
        QVariant site = m_model->data(m_model->index(row,InternalAmount::DisplayModel::Col_Site));
        QVariant siteUid = rIO.getSiteUidFromSite(site.toString());
        QVariant act = m_model->data(m_model->index(row,InternalAmount::DisplayModel::Col_Act));

        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " values =" << QString::number(cash)+ " "
                     << QString::number(cheque)+ " "
                     << QString::number(visa)+ " "
                     << QString::number(banking)+ " "
                     << QString::number(other)+ " "
                     << QString::number(due)
                     << "site uid = "+siteUid.toString()
                     << "insurance uid = "+insuranceUid.toString()
                     << "act = "+act.toString();
        QString patientUid = patient()->uuid();
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " patientUid =" << patientUid ;
        if (patientUid.isEmpty())
        {
            patientUid = "no-patient-uid";
        }
        QString patientName = patient()->data(Core::IPatient::FullName).toString();
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " patientName =" << patientName ;
        if (patientName.isEmpty())
        {
            patientName = "Patient Name";
        }
        if (manager.isMedintuxArg())
        {
            patientName = manager.getFullName();
        }
        if (manager.isFreaccountIndependant())
        {
            patientName = ui->nameEdit->text();
        }
        if (!m_freePayer.isEmpty())
        {
        	patientName = m_freePayer;
            }
        /* if (ui->freeTextCheckBox->isChecked())
    {
        FreeText freeTextDialog(this);
        if (freeTextDialog.exec()==QDialog::Accepted)
        {
            patientName = freeTextDialog.getFreeText();
            }
        ui->freeTextCheckBox->setChecked(false);
        }*/
        QHash<int,QVariant> hash;
        hash.insert(ACCOUNT_UID,"UID");
        hash.insert(ACCOUNT_USER_UID,userUuid);
        hash.insert(ACCOUNT_PATIENT_UID,patientUid);
        hash.insert(ACCOUNT_PATIENT_NAME,patientName);
        hash.insert(ACCOUNT_SITE_ID,siteUid);//AccountDB::Constants::SITES_UID
        hash.insert(ACCOUNT_INSURANCE_ID,insuranceUid);
        hash.insert(ACCOUNT_DATE,ui->dateExecution->date()/*.toString("yyyy-MM-dd")*/);
        hash.insert(ACCOUNT_MEDICALPROCEDURE_XML, QVariant());
        hash.insert(ACCOUNT_MEDICALPROCEDURE_TEXT,act);
        hash.insert(ACCOUNT_COMMENT, QVariant());
        hash.insert(ACCOUNT_CASHAMOUNT,cash);
        hash.insert(ACCOUNT_CHEQUEAMOUNT,cheque);
        hash.insert(ACCOUNT_VISAAMOUNT,visa);
        hash.insert(ACCOUNT_INSURANCEAMOUNT,banking);
        hash.insert(ACCOUNT_OTHERAMOUNT,other);
        hash.insert(ACCOUNT_DUEAMOUNT,due);
        hash.insert(ACCOUNT_DUEBY,insurance);
        hash.insert(ACCOUNT_ISVALID,0);
        hash.insert(ACCOUNT_TRACE, QVariant());
        receiptsEngine r;
        if (!r.insertIntoAccount(hash,userUuid)) {
            Utils::warningMessageBox(tr("Error inserting into AccountModel!"), tkTr(Trans::Constants::CONTACT_DEV_TEAM));
        }
    }
    m_freePayer = QString();
    clearAll(true);
}

void ReceiptViewer::saveAndQuit()
{
    save();
    QApplication::closeAllWindows();
}

void ReceiptViewer::quitFreeAccount()
{
    QApplication::closeAllWindows();
}

void ReceiptViewer::saveInThesaurus(){
    QString listOfValuesStr = m_listOfValues.join("+");
    receiptsEngine r;
    if(r.insertInThesaurus(listOfValuesStr,m_userUuid)){
        Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), listOfValuesStr+"\n"+tr(" has been saved in thesaurus!"));
    }
    m_actionTreeView->fillActionTreeView();
}

void ReceiptViewer::clearAll(bool b)
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " in clearAll ";
    if (b==false) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "Clear all is uncheckable." ;
    }
    m_listOfValues.clear();
    if (!m_modelReturnedList->removeRows(0,m_modelReturnedList->rowCount(),QModelIndex()))
    {
        LOG_ERROR("unable to remove rows");
    }
    //clear accountmodel
    if (!m_model->removeRows(0,m_model->rowCount(QModelIndex()),QModelIndex()))
    {
        LOG_ERROR("unable to remove row in accountmodel");
    }
}

QVariant ReceiptViewer::firstItemchosenAsPreferential(QString & item)
{
    QVariant variantValue = QVariant("No item");
    ReceiptsManager manager;
    if (item == "Distance rules")
    {
        variantValue = manager.m_preferredDistanceValue;
    }
    if (manager.getHashOfSites().keys().contains(item))
    {
        variantValue = manager.m_preferredSite;
    }
    if (manager.getHashOfInsurance().keys().contains(item))
    {
        variantValue = manager.m_preferredInsurance;
    }
    return variantValue;
}

void ReceiptViewer::showControlReceipts(){
    m_control->show();
}

void ReceiptViewer::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event);
    m_control->resizeControlReceipts(this);
}

void ReceiptViewer::controlReceiptsDestroyed(){
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " in controlReceiptsDestroyed " ;
}

void ReceiptViewer::userUid(){
    m_userUuid = user()->uuid();
}

/*void ReceiptViewer::createFirstTimeTxt(){
    qWarning() << __FILE__ << QString::number(__LINE__) << "create first time file." ;
    if (!m_fileFirstTime->open(QIODevice::WriteOnly))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "m_fileFirstTime cannot be created." ;
        }
    m_fileFirstTime->close();
}*/
