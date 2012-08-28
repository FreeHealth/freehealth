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
 *  Main Developers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "medicalprocedurepage.h"
#include <accountplugin/constants.h>

#include <accountbaseplugin/medicalproceduremodel.h>
#include <accountbaseplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>

#include <coreplugin/constants_icons.h>

#include <accountbaseplugin/constants.h>

#include <QCompleter>
#include <QDir>
#include <QTimer>

enum { WarnDebugMessage = false };

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;
using namespace AccountDB;
using namespace AccountDB::Constants;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }


MedicalProcedurePage::MedicalProcedurePage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("MedicalProcedurePage");
}

MedicalProcedurePage::~MedicalProcedurePage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString MedicalProcedurePage::id() const { return objectName(); }
QString MedicalProcedurePage::name() const { return tkTr(Trans::Constants::MEDICAL_PROCEDURES); }
QString MedicalProcedurePage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }
int MedicalProcedurePage::sortIndex() const {return 70;}

void MedicalProcedurePage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void MedicalProcedurePage::applyChanges()
{
    if (WarnDebugMessage)
        LOG("applyChanges");
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void MedicalProcedurePage::finish() { delete m_Widget; }

void MedicalProcedurePage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *MedicalProcedurePage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new MedicalProcedureWidget(parent);
    return m_Widget;
}

MedicalProcedureWidget::MedicalProcedureWidget(QWidget *parent) :
        QWidget(parent)
{
//    QCoreApplication::processEvents(QEventLoop::AllEvents);
    setObjectName("MedicalProcedureWidget");
    setupUi(this);
    dateEdit->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    m_db = QSqlDatabase::database(AccountDB::Constants::DB_ACCOUNTANCY);
    m_index = 0;
    m_user_uid = user()->uuid();
    m_user_fullName = user()->value(Core::IUser::FullName).toString();
    if (m_user_fullName.isEmpty()) {
        m_user_fullName = "Admin_Test";
    }
    addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    removeButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    ownersComboBox->addItem(m_user_fullName,QVariant());
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    dateEdit->setDate(QDate::currentDate());
    amountSpin->setRange(0.00,1000000000.00);
    rateSpin->setRange(0.00,100.00);

    //fill alphabet combo
    QStringList listOfAlphabet;
    //QString alphabet = tr("abcdefghijklmnopqrstuvwxyz");
    QString alphabet = tr("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    for (int i = 0; i < alphabet.size(); i += 1)
    {
        QString letter = alphabet.mid(i,1);
        listOfAlphabet << letter;
        }
    alphabetBox->addItems(listOfAlphabet);

    const QString insuranceInfo = tr("Insurances are created in another preference widget.");
    insuranceBox->setToolTip(insuranceInfo);

    /*const QString filterMaxNumber = QString("%1 < '%2'").arg("MP_ID","10");
    modelMP->setFilter(filterMaxNumber);
    modelMP->select();*/
    userUidLabel->setText(m_user_uid);
     //mpComboBox->setModel(modelMP);
    //mpComboBox->setMaxCount(20);
    //mpComboBox->setModelColumn(AccountDB::Constants::MP_NAME);
    //setDatasToUi();


//    connect(createDefault, SIGNAL(clicked()), SLOT(createDefaultMedicalProcedures()));
}

MedicalProcedureWidget::~MedicalProcedureWidget()
{
    //saveModel();
    //delete m_progressDialog;
}

void MedicalProcedureWidget::fillHugeWidgets(){

    fillTypeCompletionList();
    m_hashInsuranceBox = fillHashOfInsurances();
    QStringList listInsurances = m_hashInsuranceBox.values();
    insuranceBox->addItems(listInsurances);
    fillMPCombo();
}

void MedicalProcedureWidget::setDatasToUi()
{
    //m_Mapper->setCurrentIndex(900);
}

void MedicalProcedureWidget::on_alphabetBox_currentIndexChanged(const QString & text){
    Q_UNUSED(text);
    fillMPCombo();
}

void MedicalProcedureWidget::on_mpComboBox_currentIndexChanged(const QString & text)
{
    //saveModel();
    m_index = 0;
    int row = 0;
    QSqlQuery q(m_db);
    const QString req = QString("SELECT %1 FROM %2 ").arg("MP_USER_UID,NAME,ABSTRACT,TYPE,AMOUNT,REIMBOURSEMENT,DATE",
                                                          "medical_procedure");
    if (!q.exec(req)) {
        LOG_QUERY_ERROR(q);
    }
    while (q.next()) {
        QString str = q.value(1).toString();
        if (str == text) {
            m_index = row;
            //ownersComboBox->setEditText(q.value(WN_MP_USER_UID).toString());
            name->setText(q.value(WN_NAME).toString());
            abstractEdit->setText(q.value(WN_ABSTRACT).toString());
            type->setText(q.value(WN_TYPE).toString());
            amountSpin->setValue(q.value(WN_AMOUNT).toDouble());
            rateSpin->setValue(q.value(WN_REIMBOURSEMENT).toDouble());
            dateEdit->setDate(q.value(WN_DATE).toDate());
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " index =" << QString::number(row) ;
        }
        ++row;
    }

    //m_Mapper->setCurrentIndex(m_index);

    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << "index   =" << QString::number(m_index);
}

void MedicalProcedureWidget::on_addButton_clicked()
{
    MedicalProcedureModel *modelMP = new MedicalProcedureModel(this);
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount1 =" << QString::number(modelMP->rowCount());
    int numberOfRows = modelMP->rowCount();
    if (!modelMP->insertRow(numberOfRows,QModelIndex()))
        LOG_ERROR("Unable to add row");
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount2 =" << QString::number(modelMP->rowCount());
    name->setText("");
    abstractEdit->setText("");
    type->setText("");
    amountSpin->setValue(0.00);
    rateSpin->setValue(70.00);
    dateEdit->setDate(QDate::currentDate());
}

/*void MedicalProcedureWidget::on_save_clicked()
{
    saveModel();
}*/

//void MedicalProcedureWidget::on_name_textChanged(const QString & text){
//    mpComboBox->setItemText(mpComboBox->currentIndex(),text);
//}

void MedicalProcedureWidget::on_removeButton_clicked()
{
    QSqlQuery q(m_db);
    const QString nameStr = name->text();
    const QString condition = QString("%1 = '%2' AND %3 = '%4'")
            .arg("NAME",nameStr,"DATE",dateEdit->date().toString("yyyy-MM-dd"));
    const QString req = QString("DELETE FROM %1 WHERE %2").arg("medical_procedure",condition);
    if (!q.exec(req)) {
        LOG_QUERY_ERROR(q);
    } else {
        Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), nameStr+tr(" has been deleted."));
        fillMPCombo();
    }


    /*if (!modelMP->removeRow(mpComboBox->currentIndex()))
    {
          LOG_ERROR("Unable to remove row");
        }
    mpComboBox->setCurrentIndex(modelMP->rowCount() - 1);*/
}

void MedicalProcedureWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    if (WarnDebugMessage)
        qWarning() << __FILE__ << QString::number(__LINE__) << "saving" ;
    /*if (!modelMP->submit()) {
        LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("medical_procedures")));
        Utils::warningMessageBox(tr("Can not submit medical procedure to your personnal database."),
                                 tr("An error occured during medical procedures saving. Data are corrupted."));
    }
        QString typeText = type->text();
        setCompletionList(typeText);
        QString textAbstract = abstractEdit->text();
        setCompletionAbstractList(textAbstract);
        connect(name,SIGNAL(textEdited(const QString &)),mpComboBox,SLOT(setEditText(const QString &)));*/
        save();
        update();
    QApplication::restoreOverrideCursor ();
}

void MedicalProcedureWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
//    Utils::Log::addMessage("MedicalProcedureWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("MedicalProcedureWidget"));
//    s->sync();
}

void MedicalProcedureWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        addButton->setToolTip(tkTr(Trans::Constants::FILENEW_TEXT));
        removeButton->setToolTip(tkTr(Trans::Constants::REMOVE_TEXT));
//        int s = defaultCombo->currentIndex();
//        defaultCombo->clear();
//        defaultCombo->addItem(tkTr(Trans::Constants::NO));
//        defaultCombo->addItem(tkTr(Trans::Constants::YES));
//        defaultCombo->setCurrentIndex(s);
        break;
    default:
        break;
    }
}

void MedicalProcedureWidget::showEvent(QShowEvent *event){
    Q_UNUSED(event);
    if (WarnDebugMessage)
        qWarning() << __FILE__ << QString::number(__LINE__) << " medical procedures activated "   ;
    fillHugeWidgets();
    update();
}

void MedicalProcedureWidget::setCompletionList(const QString & text){
    m_completionList << text;
    m_completionList.removeDuplicates();
}

void MedicalProcedureWidget::setCompletionAbstractList(const QString & text){
    m_completionAbstractList << text;
    m_completionAbstractList.removeDuplicates();
}

void MedicalProcedureWidget::on_type_textChanged(const QString & text){
    Q_UNUSED(text);
    QCompleter *c = new QCompleter(m_completionList,this);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    type->setCompleter(c);
}

void MedicalProcedureWidget::on_abstractEdit_textChanged(const QString & text){
    Q_UNUSED(text);
    QCompleter *c = new QCompleter(m_completionAbstractList,this);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    abstractEdit->setCompleter(c);
}

void  MedicalProcedureWidget::fillMPCombo(){
    const QString letter = alphabetBox->currentText();
    QSqlQuery q(m_db);
    const QString filter = QString("NAME LIKE '%1'").arg(letter+"%");
    const QString req = QString("SELECT %1 FROM %2 WHERE %3").arg("NAME","medical_procedure",filter);
    if (!q.exec(req))
        LOG_QUERY_ERROR(q);
    QStringList list;
    while (q.next()) {
        QString str = q.value(0).toString();
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " strItem =" << str ;
        list << str;
    }
    mpComboBox->clear();
    mpComboBox->addItems(list);
}

void MedicalProcedureWidget::save(){
    const QString nameStr = name->text().trimmed();
    bool test = false;
    QSqlQuery q(m_db);
    const QString req = QString("SELECT %1 FROM %2").arg("NAME","medical_procedure");
    if (!q.exec(req))
        LOG_QUERY_ERROR(q);
    while (q.next()) {
        QString str = q.value(0).toString();
        str = str.trimmed();
        if (str==nameStr) {
              test = true;
        }
    }
    if (test) {
         QSqlQuery qInsert(m_db);
         const QString medicalProcedure = tr("medical_procedure");
         const QString abstractUpdate = QString("ABSTRACT = '%1'").arg(abstractEdit->text());
         const QString typeUpdate = QString("TYPE = '%1'").arg(type->text());
         const QString amountSpinUpdate = QString("AMOUNT = '%1'").arg(QString::number(amountSpin->value()));
         const QString rateSpinUpdate = QString("REIMBOURSEMENT = '%1'").arg(QString::number(rateSpin->value()));
         const QString dateUpdate = QString("DATE = '%1'").arg(dateEdit->date().toString("yyyy-MM-dd"));
         const QString condition = QString("NAME = '%1' AND DATE = '%2'")
                              .arg(nameStr,dateEdit->date().toString("yyyy-MM-dd"));
         const QString reqInsert = QString("UPDATE %1 SET %2  , %3 , %4 , %5, %6 WHERE %7 ")
                              .arg(medicalProcedure,
                                   abstractUpdate,
                                   typeUpdate,
                                   amountSpinUpdate,
                                   rateSpinUpdate,
                                   dateUpdate,
                                   condition);
         if (!qInsert.exec(reqInsert))
             LOG_QUERY_ERROR(qInsert);
    } else {
        MedicalProcedureModel *modelMP = new MedicalProcedureModel(this);
        QString owner = userUidLabel->text();
        int insuranceUid = m_hashInsuranceBox.key(insuranceBox->currentText());
        QString abstract =  abstractEdit->text();
        QString typeStr =  type->text();
        QVariant amount =  amountSpin->value();
        QVariant rate =  rateSpin->value();
        QVariant date =   dateEdit->date();
        bool test = true;
        int numberOfRows = modelMP->rowCount()  ;
        if (!modelMP->insertRows(numberOfRows,1,QModelIndex()))
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << "unable to insert row to MP" ;
            }
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " numberOfRows =" << QString::number(numberOfRows) ;
        if (!modelMP->setData(modelMP->index(numberOfRows,MP_USER_UID),owner,Qt::EditRole)) {
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__)  << "a" ;
            test = false;
        }
        if (!modelMP->setData(modelMP->index(numberOfRows,MP_INSURANCE_UID),insuranceUid,Qt::EditRole)) {
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__)  << "a" ;
            test = false;
        }
        if (!modelMP->setData(modelMP->index(numberOfRows,MP_NAME),nameStr,Qt::EditRole)) {
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << "b"  ;
            test = false;
        }
        if (!modelMP->setData(modelMP->index(numberOfRows,MP_ABSTRACT),abstract,Qt::EditRole)) {
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__)  << "c" ;
            test = false;
        }
        if (!modelMP->setData(modelMP->index(numberOfRows,MP_TYPE),typeStr,Qt::EditRole)) {
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__)  << "d" ;
            test = false;
        }
        if (!modelMP->setData(modelMP->index(numberOfRows,MP_AMOUNT),amount,Qt::EditRole)) {
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__)  << "e" ;
            test = false;
        }
        if (!modelMP->setData(modelMP->index(numberOfRows,MP_REIMBOURSEMENT),rate,Qt::EditRole)) {
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << "f"  ;
            test = false;
        }
        if (!modelMP->setData(modelMP->index(numberOfRows,MP_DATE),date,Qt::EditRole)) {
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << "g"  ;
            test = false;
        }
        if (!modelMP->submit()) {
            LOG_ERROR("unable to submit model");
            test = false;
        }
        if (test == false) {
            LOG_ERROR("Error inserting data: " + modelMP->lastError().text());
        }
        fillMPCombo();
    }
}

void MedicalProcedureWidget::fillTypeCompletionList(){
    QSqlQuery q(m_db);
    const QString req = QString("SELECT %1 FROM %2").arg("TYPE","medical_procedure");
    if (!q.exec(req))
        LOG_QUERY_ERROR(q);
    while (q.next()) {
        QString str = q.value(0).toString();
        m_completionList << str;
    }
    m_completionList.removeDuplicates();
}

QHash<int,QString> MedicalProcedureWidget::fillHashOfInsurances(){
    QHash<int,QString> hash;
    QSqlQuery q(m_db);
    const QString req = QString("SELECT %1,%2 FROM %3").arg("INSURANCE_UID","NAME","insurance");
    if (!q.exec(req))
        LOG_QUERY_ERROR(q);
    while (q.next()) {
        int insuranceUid = q.value(0).toInt();
        QString name = q.value(1).toString();
        hash.insertMulti(insuranceUid,name);
    }
    return hash;
}

//static QString getCsvDefaultFile()
//{
//    QString sqlDirPath = settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/account";
//    QString defaultString = ("medical_procedure");
//    QDir dir(sqlDirPath);
//    if (!dir.exists())
//        return QString();
//    QStringList nameFilters;
//    nameFilters << "*"+QLocale().name()+".csv";
//    dir.setNameFilters(nameFilters);
//    QStringList listOfFiles = dir.entryList(nameFilters);
//    QString fileName ;//= QString("medical_procedure_%1.csv").arg(QLocale().name());
//    QString filesOfDir;
//    foreach(filesOfDir,listOfFiles){
//        if (filesOfDir.contains(defaultString))
//        {
//        	  fileName = dir.absolutePath() + QDir::separator() +filesOfDir;
//        	  if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " fileName =" << fileName ;
//            }
//        }
//    QFile file(fileName);
//    if (!file.exists())
//        return QString();
//    return file.fileName();
//}

//void MedicalProcedureWidget::createDefaultMedicalProcedures()
//{
//    int max = numberOfLinesForProgressBar();
//    m_progressDialog = new QProgressDialog(tr("Wait ..."), tr("Abort"),0,max,this);
//    m_progressDialog->setWindowModality(Qt::WindowModal);
//    m_progressDialog->setAutoReset(true);
//    m_progressDialog->setAutoClose(true);
//    fillEmptyMPModel();
//}

//QStandardItemModel *MedicalProcedureWidget::MedicalProcedureModelByLocale()
//{
//    QApplication::setOverrideCursor(Qt::WaitCursor);
//    QStandardItemModel *model = new QStandardItemModel;
//    QString csvFileName = getCsvDefaultFile();
//    if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " csvFileName =" << csvFileName ;
//    QFile file(getCsvDefaultFile());
//    // some validity checking
//    if (!file.exists()) {
//        LOG_ERROR(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(QLocale().name() + " " + tr("Medical_procedure.")));
//        return model;
//    }
//    if (!file.open(QFile::ReadOnly | QFile::Text)) {
//        LOG_ERROR(tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(file.fileName()));
//        return model;
//    }

//    // read the content with UTF8 coding system
//    QTextStream stream(&file);
//    stream.setCodec("UTF-8");
//    // skip first line
//    //stream.readLine();
//    int row = 0;
//    while (!stream.atEnd())
//    {
//        //int row = 0;
//        QString line = stream.readLine();
//        QStringList listOfSeparators;
//        listOfSeparators << ",\"" << ";\"" << QString("\t\"")
//                         << ",''" << ";''" << QString("\t''");
//        QString separator = ";\"";
//        /*QString separatorStr;
//        foreach(separatorStr,listOfSeparators){
//            if (line.contains(separatorStr)){
//                separator = separatorStr;
//                }
//            }*/
//        if (!line.contains("MP_UUID")){
//            //"MP_ID","MP_UUID","MP_USER_UID","MP_INSURANCE_UID","NAME","ABSTRACT","TYPE","AMOUNT","REIMBOURSEMENT","DATE"
//            QList<QStandardItem*> listOfItemsData;
//            QStringList listOfItems;
//            listOfItems = line.split(separator);
//            for (int i = 0; i < AccountDB::Constants::MP_MaxParam ; i += 1){
//                //model->setData(model->index(row,i),listOfItems[i],Qt::EditRole);
//        	QStandardItem * item = new QStandardItem;
//        	//if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " listOfItems[i] =" << listOfItems[i] ;
//        	QString itemOfList = listOfItems[i];
//        	itemOfList.remove("\"");
//        	item->setData(itemOfList);
//        	listOfItemsData << item;
//        	}
//            model->appendRow(listOfItemsData);
//            ++row;
//            //if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(row) ;
//            }
//    }

//    return model;
//}

//bool MedicalProcedureWidget::fillEmptyMPModel()
//{
//    bool test = false;
//    QStandardItemModel * model = MedicalProcedureModelByLocale();
//    int availModelRows = model->rowCount();
//    if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " availModelRows = " << QString::number(availModelRows) ;
//    QString strList;
//    for (int i = 0; i < availModelRows; i += 1){
//        m_progressDialog->setValue(i);
//        //if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " i =" << QString::number(i) ;
//        if (!modelMP->insertRows(modelMP->rowCount(),1,QModelIndex())) {
//            qWarning() << __FILE__ << QString::number(__LINE__) << QString::number(modelMP->rowCount()) ;
//        }
//        QString strValues;
//        for (int j = 0; j < AccountDB::Constants::MP_MaxParam ; j += 1) {
//            QStandardItem * item = model->itemFromIndex(model->index(i,j));
//            QVariant value = item->data();
//            //todo, semantics
//            if (value.canConvert(QVariant::String)) {
//                QString strValue = value.toString().replace("'","''");
//                value = QVariant::fromValue(strValue);
//            }
//            strValues += value.toString()+" ";
//            //if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << value ;
//            //if (WarnDebugMessage)
//                  qDebug() << __FILE__ << QString::number(__LINE__) << "modelMP->rowCount() =" << QString::number(modelMP->rowCount()) ;
//            if (!modelMP->setData(modelMP->index(modelMP->rowCount()-1,j),value,Qt::EditRole)) {
//                qWarning() << __FILE__ << QString::number(__LINE__) << "data not inserted !" ;
//            }
//        }
//        strList += strValues+"\n";
//        test = modelMP->submit();
//    }
//    m_progressDialog->setValue(numberOfLinesForProgressBar());

//    //m_progressDialog->close();
//    //if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " values =\n" << strList;
//    QApplication::restoreOverrideCursor();
//    return test;
//}

//int MedicalProcedureWidget::numberOfLinesForProgressBar()
//{
//    QString filePathAndName = getCsvDefaultFile();
//    QString fileName = filePathAndName.split(QDir::separator()).last();
//    if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " fileName =" << fileName ;
//    fileName.remove("_"+QLocale().name());
//    fileName.remove(".csv");
//    QStringList listFromFileName = fileName.split("_");
//    QString numberOfLines = listFromFileName.last();
//    if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " numberOfLines =" << numberOfLines ;
//    int max = numberOfLines.toInt();
//    if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " max =" << QString::number(max) ;
//    return max;
//}

