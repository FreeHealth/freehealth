/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,       *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "choiceDialog.h"
#include "receiptsmanager.h"
#include "receiptsIO.h"
#include "ui_ChoiceDialog.h"
#include "constants.h"
#include "tools.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QRadioButton>
#include <QBrush>
#include <QColor>
#include <QMouseEvent>

enum { WarnDebugMessage = false };

using namespace ChoiceActions;
using namespace Trans::ConstantTranslations;
using namespace ReceiptsConstants;
using namespace Tools;

static inline Core::IUser *user() { return Core::ICore::instance()->user(); }

treeViewsActions::treeViewsActions(QWidget *parent):QTreeView(parent){
    m_deleteThesaurusValue = new QAction(tr("Delete value"),this);
    m_choosepreferredValue = new QAction(tr("Choose value as preferred"),this);
    connect(m_choosepreferredValue,SIGNAL(triggered(bool)),this,SLOT(choosepreferredValue(bool)));
    connect(m_deleteThesaurusValue,SIGNAL(triggered(bool)),this,SLOT(deleteBox(bool)));
    }

treeViewsActions::~treeViewsActions(){}

void treeViewsActions::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton){
        if(isChildOfThesaurus()){
            blockSignals(true);
            m_menuRightClick = new QMenu(this);
            m_menuRightClick -> addAction(m_choosepreferredValue);
            m_menuRightClick-> addAction(m_deleteThesaurusValue);
            m_menuRightClick->exec(event->globalPos());
            blockSignals(false);
        }

    }
    if(event->button() == Qt::LeftButton){
        blockSignals(false);
        QTreeView::mousePressEvent(event);
    }
}

void treeViewsActions::deleteBox(bool b){
    Q_UNUSED(b);
    bool yes = Utils::yesNoMessageBox(tr("Do you want to delete selected item?"),
                           tr("Do you want to delete selected item?"));
    if (yes) {
        QModelIndex index = currentIndex();
        deleteItemFromThesaurus(index);
    }
}

void treeViewsActions::choosepreferredValue(bool b){
    Q_UNUSED(b);
    bool yes = Utils::yesNoMessageBox(tr("Do you want to set this item as preferred value?"),
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

bool treeViewsActions::isChildOfThesaurus(){
    bool ret = true;
    QModelIndex current = currentIndex();
    QModelIndex indexParent = treeModel()->parent(current);
    QString dataParent = treeModel()->data(indexParent).toString();
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " dataParent =" << dataParent ;
    if (dataParent != "Thesaurus")
    {
          ret = false;
        }
    return ret;
}

void treeViewsActions::fillActionTreeView()
{
    m_actionsTreeModel = new QStandardItemModel;
    ReceiptsTools rt;
    QStringList listOfMainActions;
    QMap<int,QString> parametersMap;
    parametersMap.insert(DEBTOR_ITEM,rt.getStringFromRows(DEBTOR_ITEM));
    parametersMap.insert(SITES_ITEM,rt.getStringFromRows(SITES_ITEM));
    parametersMap.insert(DISTANCE_RULES_ITEM,rt.getStringFromRows(DISTANCE_RULES_ITEM));
    listOfMainActions = parametersMap.values();
    QList<int> listOfEnums;
    listOfEnums << DEBTOR_ITEM << SITES_ITEM << DISTANCE_RULES_ITEM;
    //insert items from tables if available
    QMap<QString,QString> mapSubItems;
    ReceiptsManager manager;
    //QString strKeysParameters;
    for (int item = 0; item < listOfEnums.size();++item){
        QString table = rt.getStringFromRows(item);
        QStringList listOfItemsOfTable;
        QString null = QString();
        listOfItemsOfTable = manager.getParametersData(null,item).keys(); // QHash<QString,QVariant> name,uid
        QString strItemsOfTable;
        foreach(strItemsOfTable,listOfItemsOfTable){
            mapSubItems.insertMulti(table,strItemsOfTable);
            }
        //default values if unavailables :
        if (listOfItemsOfTable.size()<1)
        {
              if (item == DEBTOR_ITEM)
              {
                   mapSubItems.insertMulti(table,"Patient");
                       mapSubItems.insertMulti(table,"CPAM28");
                  }
              else if (item == SITES_ITEM)
              {
                   mapSubItems.insertMulti(table,"cabinet");
                       mapSubItems.insertMulti(table,"clinique");
                  }
              else if (item == DISTANCE_RULES_ITEM)
              {
                  mapSubItems.insertMulti(table,"DistPrice");
                  }
              else
              {
                   qWarning() << __FILE__ << QString::number(__LINE__)
                   << " No default value for "<< table ;
                   }
            }
    }
    QStandardItem *parentItem = treeModel()->invisibleRootItem();
    QString strMainActions;

    foreach(strMainActions,listOfMainActions){
        if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " strMainActions =" << strMainActions ;
        QStandardItem *actionItem = new QStandardItem(strMainActions);
        int parentEnumItem = 0;
        parentEnumItem = rt.getEnumItemFromRow(strMainActions);
        //treeViewsActions colors
        if (parentEnumItem == DEBTOR_ITEM)
        {
              QBrush green(Qt::darkGreen);
                  actionItem->setForeground(green);
            }

        else if (parentEnumItem == SITES_ITEM)
        {
              QBrush green(Qt::darkGreen);
                  actionItem->setForeground(green);
            }

        else if (parentEnumItem == DISTANCE_RULES_ITEM)
        {
              QBrush green(Qt::darkGreen);
                  actionItem->setForeground(green);
            }
        else{
                  qWarning() << __FILE__ << QString::number(__LINE__) << "Error color treeViewsActions." ;
        }

        parentItem->appendRow(actionItem);
        QStringList listSubActions;
        listSubActions = mapSubItems.values(strMainActions);
        QString strSubActions;
        foreach(strSubActions,listSubActions){
            if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " strSubActions =" << strSubActions ;
            QStandardItem *subActionItem = new QStandardItem(strSubActions);
            actionItem->appendRow(subActionItem);
        }
    }
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__)  ;
    setHeaderHidden(true);
    setStyleSheet("background-color: rgb(201, 201, 201)");
   // actionsTreeView->setStyleSheet("foreground-color: red");
    setModel(treeModel());
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__)  ;
}

bool treeViewsActions::deleteItemFromThesaurus(QModelIndex &index){
    bool ret = true;
    QString data = index.data().toString();
    receiptsEngine r;
    const QString userUid = user()->uuid();
    if (!r.deleteFromThesaurus(data,userUid)) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Cannot delete in thesaurus: ")+data);
        ret = false;
    }
    fillActionTreeView();
    return ret;
}

QModelIndex treeViewsActions::indexWithItem(int row)
{
    QModelIndex index;
    index = treeModel()->indexFromItem(treeModel()->item(row));
    return index;
}



///////////////////////////////////////////////////////////////////////////
//////// CHOICEDIALOG
///////////////////////////////////////////////////////////////////////////

using namespace ReceiptsConstants;

choiceDialog::choiceDialog(QWidget * parent,bool roundtrip, bool freevalue, QString preferredValue):QDialog(parent),ui(new Ui::ChoiceDialog){
    ui->setupUi(this);
    ui->distanceDoubleSpinBox->hide();
    ui->distanceGroupBox->hide();
    ui->freeBox->hide();
    ui->freeLabel->hide();
    ui->freeEdit->hide();
    ui->freeValueLabel->hide();
    ui->freeValueSpinBox->hide();
    m_percent = 100.00;
    //m_percentValue = 100.00;
    ReceiptsManager manager;
    manager.getpreferredValues();
    m_hashPercentages = manager.getPercentages();
    m_quickInt = m_hashPercentages.keys().last();
    QString textOfPrefValue = manager.getStringPerferedActAndValues(preferredValue);
    QString labelText = "<html><body><font color = red  font size = 3>"+textOfPrefValue+"</font></body></html>";
    if (preferredValue.isEmpty())
    {
          labelText = QString("");
        }
    ui->label->setText(labelText);
    ui->percentDoubleSpinBox->setRange(0.00,100.00);
    ui->percentDoubleSpinBox->setValue(100.00);
    ui->percentDoubleSpinBox->setSingleStep(0.10);
    ui->percentDoubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    if (roundtrip)
    {
        ui->distanceDoubleSpinBox->show();
        ui->distanceGroupBox->show();
        ui->distanceDoubleSpinBox->setRange(0.00,100000.00);
        ui->distanceDoubleSpinBox->setSingleStep(0.10);
        }
    if (freevalue)
    {
        ui->freeBox->show();
        ui->freeLabel->show();
        ui->freeEdit->show();
        ui->freeValueLabel->show();
        ui->freeValueSpinBox->show();
        ui->freeValueSpinBox->setRange(0.00,100000.00);
        ui->freeValueSpinBox->setSingleStep(0.10);
        }
    //treeViewsActions
    m_actionTreeView = new treeViewsActions(this);
    QVBoxLayout *vbox = new QVBoxLayout;
                 vbox-> addWidget(m_actionTreeView);
    ui->paramsGroupBox->setLayout(vbox);
    m_actionTreeView->fillActionTreeView();
    QModelIndex indexDebtor = m_actionTreeView->indexWithItem(treeViewsActions::DEBTOR);
    m_actionTreeView->expand(indexDebtor);
    //preferential choices in the tree view.
    QString site = tr("Sites");
    QString distRule = tr("Distance rules");
    QString debtor = tr("Debtor");
    m_siteUid = firstItemchosenAsPreferential(site);
    m_distanceRuleValue = firstItemchosenAsPreferential(distRule).toDouble();
    m_distanceRuleType = manager.getpreferredDistanceRule().toString();
    m_insurance = firstItemchosenAsPreferential(debtor);
    m_insuranceUid = manager.m_preferredInsuranceUid;
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " m_insuranceUid =" << m_insuranceUid.toString() ;
    m_modelChoicePercentDebtorSiteDistruleValues = new QStandardItemModel(0,returningModel_MaxParam);
    m_row = 0;
    m_timerUp = new QTimer(this);
    m_timerDown = new QTimer(this);
   // connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(beforeAccepted()));
    connect(ui->okButton,SIGNAL(pressed()),this,SLOT(beforeAccepted()));
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(reject()));
    connect(ui->percentDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(value(double)));
    connect(ui->plusButton,SIGNAL(pressed()),this,SLOT(valueUp()));
    connect(ui->plusButton,SIGNAL(released()),this,SLOT(valueUpStop()));
    connect(ui->lessButton,SIGNAL(pressed()),this,SLOT(valueDown()));
    connect(ui->lessButton,SIGNAL(released()),this,SLOT(valueDownStop()));
    connect(ui->plusConstButton,SIGNAL(clicked()),this,SLOT(quickPlus()));
    connect(ui->lessConstButton,SIGNAL(clicked()),this,SLOT(quickLess()));
    connect(m_actionTreeView,SIGNAL(clicked(const QModelIndex&)),this,SLOT(actionsOfTreeView(const QModelIndex&)));
}

choiceDialog::~choiceDialog(){
    delete m_timerUp;
    delete m_timerDown;
}

double choiceDialog::getDistanceNumber(const QString & data){
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " data =" << data  ;
    receiptsEngine recIO;
    double dist = 0.00;
    double minDistance = recIO.getMinDistanceValue(data);
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " minDistance =" << QString::number(minDistance) ;
    dist = ui->distanceDoubleSpinBox->value() - minDistance;
    if (dist < 0.00)
    {
          dist = 0.00;
        }
    return dist;
}

int choiceDialog::returnChoiceDialog(){
    int ret = 0;
        if (ui->cashButton->isChecked())
    {
        ret = Cash;
        }
        if (ui->checkButton->isChecked())
    {
        ret = Check;
        }
        if (ui->visaButton->isChecked())
    {
        ret = Visa;
        }
        if (ui->bankingButton->isChecked())
    {
        ret = Banking;
        }
        if (ui->otherButton->isChecked())
    {
        ret = Other;
        }
        if (ui->dueButton->isChecked())
    {
        ret = Due;
        }
    return ret;
}

void choiceDialog::value(double val){
    m_percent = val;
}

void choiceDialog::valueUp(){
    connect(m_timerUp,SIGNAL(timeout()),ui->percentDoubleSpinBox,SLOT(stepUp()));
    m_timerUp->start(10);
}

void choiceDialog::valueDown(){
    connect(m_timerDown,SIGNAL(timeout()),ui->percentDoubleSpinBox,SLOT(stepDown()));
    m_timerDown->start(10);
}


void choiceDialog::valueUpStop(){
    m_timerUp->stop();
}

void choiceDialog::valueDownStop(){
    m_timerDown->stop();
}


void choiceDialog::quickPlus(){
    if(m_quickInt == m_hashPercentages.keys().last())
        return;
    else{
        m_quickInt++;
        m_percent = m_hashPercentages.value(m_quickInt).toDouble();
    }
    ui->percentDoubleSpinBox->setValue(m_percent);
}

void choiceDialog::quickLess(){
    if(m_quickInt == 1)
        return;
    /*else if(m_percent == 100){
            m_percent = list[0].toDouble();
        }*/
    else {
        m_quickInt--;
        m_percent = m_hashPercentages.value(m_quickInt).toDouble();
    }
    ui->percentDoubleSpinBox->setValue(m_percent);
}

double choiceDialog::returnPercentValue(){
    return m_percent;
}

QList<double> choiceDialog::listOfPercentValues(){
    return m_listOfPercentValues;
}

/*! \brief called when the user clicks the ok button
 *
 * Does a few checks like percentage == 100, etc.
 */
void choiceDialog::beforeAccepted(){
     receiptsEngine rIO;

     if (WarnDebugMessage)
         qDebug() << __FILE__ << QString::number(__LINE__) << " m_insuranceUid =" << QString::number(m_insuranceUid.toInt());
     QString debtor = rIO.getStringFromInsuranceUid(m_insuranceUid);
     if (WarnDebugMessage)
         qDebug() << __FILE__ << QString::number(__LINE__) << " debtor =" << debtor;


     if (m_percent != 100.00) {
         bool yes = Utils::yesNoMessageBox(tr("The percentage value is lower than 100%."),
                                           tr("Do you want to choose another percentage?"),
                                           0,
                                           tr("Unusual percentage"));
         if (yes) {
             int row = m_modelChoicePercentDebtorSiteDistruleValues->rowCount();
             if (WarnDebugMessage)
                 LOG_ERROR("row = " + QString::number(row));
             m_modelChoicePercentDebtorSiteDistruleValues->insertRows(row,1,QModelIndex());
             m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(row,TYPE_OF_CHOICE),returnChoiceDialog(),Qt::EditRole);
             m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(row,PERCENTAGE),m_percent,Qt::EditRole);
             m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(row,DEBTOR),debtor,Qt::EditRole);
             m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(row,SITE),m_siteUid,Qt::EditRole);
             m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(row,DISTRULES),m_distanceRuleType,Qt::EditRole);
             m_modelChoicePercentDebtorSiteDistruleValues->submit();
             if (WarnDebugMessage)
                 LOG_ERROR("rowCount = " + QString::number(m_modelChoicePercentDebtorSiteDistruleValues->rowCount()));
             //++m_row;

         } else {
             int row = m_modelChoicePercentDebtorSiteDistruleValues->rowCount();
             if (WarnDebugMessage)
                 LOG_ERROR("row = " + QString::number(row));
             m_modelChoicePercentDebtorSiteDistruleValues->insertRows(row,1,QModelIndex());
             m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(row,TYPE_OF_CHOICE),returnChoiceDialog(),Qt::EditRole);
             m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(row,PERCENTAGE),m_percent,Qt::EditRole);
             m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(row,DEBTOR),debtor,Qt::EditRole);
             m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(row,SITE),m_siteUid,Qt::EditRole);
             m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(row,DISTRULES),m_distanceRuleType,Qt::EditRole);
             m_modelChoicePercentDebtorSiteDistruleValues->submit();

             accept();
         }
     }
     else
     {
         if (WarnDebugMessage)
             LOG_ERROR("in percentage == 100.00");
         m_modelChoicePercentDebtorSiteDistruleValues->insertRows(m_row,1,QModelIndex());
         m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,TYPE_OF_CHOICE),returnChoiceDialog(),Qt::EditRole);
         m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,PERCENTAGE),m_percent,Qt::EditRole);
         m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,DEBTOR),debtor,Qt::EditRole);
         m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,SITE),m_siteUid,Qt::EditRole);
         m_modelChoicePercentDebtorSiteDistruleValues->setData(m_modelChoicePercentDebtorSiteDistruleValues->index(m_row,DISTRULES),m_distanceRuleType,Qt::EditRole);
         accept();
     }
}

QStandardItemModel * choiceDialog::getChoicePercentageDebtorSiteDistruleModel(){
     return m_modelChoicePercentDebtorSiteDistruleValues;
}

QVariant choiceDialog::firstItemchosenAsPreferential(QString & item)
{
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " item =" << item ;
    QVariant variantValue = QVariant("No item");
    ReceiptsManager manager;
    if (item == tr("Distance rules"))
    {
          variantValue = manager.m_preferredDistanceValue;
        }
    if (item == tr("Sites"))
    {
          variantValue = manager.m_preferredSite;
        }
    if (item== tr("Debtor"))
    {
          variantValue = manager.m_preferredInsurance;
        }
    return variantValue;
}

void choiceDialog::actionsOfTreeView(const QModelIndex &index){
    QString data = index.data(Qt::DisplayRole).toString();
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " data =" << data;
    ReceiptsManager manager;
    QHash<QString,QString> hashOfValues;
    //int typeOfPayment = ReceiptsConstants::Cash;
    //double percentage = 100.00;
    QVariant  debtor;
    QVariant site;
    QVariant distrules;

    if (manager.getDistanceRules().keys().contains(data))
    {
          m_distanceRuleValue = manager.getDistanceRules().value(data).toDouble();
          m_distanceRuleType = data;
          if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " m_distanceRuleValue =" << QString::number(m_distanceRuleValue) ;
          if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " m_distanceRuleType =" << m_distanceRuleType ;
        }
    if (manager.getHashOfSites().keys().contains(data))
    {
          m_siteUid = manager.getHashOfSites().value(data);
          if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " m_siteUid =" << m_siteUid.toString() ;
        }
    if (manager.getHashOfInsurance().keys().contains(data))
    {
          m_insuranceUid = manager.getHashOfInsurance().value(data);
          if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " m_insuranceUid =" << m_insuranceUid.toString() ;
        }
        //actionTreeView->reset();
}

QString choiceDialog::getFreeText()
{
    return ui->freeEdit->text();
}

QString choiceDialog::getFreeValue()
{
    return QString::number(ui->freeValueSpinBox->value());
}
