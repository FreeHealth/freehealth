/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors : Pierre-Marie Desombre,<pm.desombre@medsyn.fr>          *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
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

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  AccountUserPage  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
MedicalProcedurePage::MedicalProcedurePage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("MedicalProcedurePage"); }

MedicalProcedurePage::~MedicalProcedurePage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString MedicalProcedurePage::id() const { return objectName(); }
QString MedicalProcedurePage::name() const { return tkTr(Trans::Constants::MEDICAL_PROCEDURES); }
QString MedicalProcedurePage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }

void MedicalProcedurePage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void MedicalProcedurePage::applyChanges()
{qDebug() << __FILE__ << QString::number(__LINE__) << " applyChanges ";
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
        QWidget(parent), m_Model(0), m_Mapper(0)
{
    setObjectName("MedicalProcedureWidget");
    setupUi(this);
    m_user_uid = user()->value(Core::IUser::Uuid).toString();
    m_user_fullName = user()->value(Core::IUser::FullName).toString();
    if (m_user_fullName.isEmpty()) {
        m_user_fullName = "Admin_Test";
    }
    save->hide();
    addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    addButton->setText("New");
    removeButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    removeButton->setText("Delete");
    ownersComboBox->addItem(m_user_fullName,QVariant());
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    //dateEdit->setDate(QDate::currentDate());
    m_Model = new AccountDB::MedicalProcedureModel(this);
        /** \todo  m_Model->setUserUuid(); */
    QLabel *mpUidLabel = new QLabel(this);
    mpUidLabel->setText("NULL");
    mpUidLabel->hide();
    userUidLabel->setText(m_user_uid);
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    //m_Mapper->addMapping(mpIDLabel, AccountDB::Constants::MP_ID, "ID");
    m_Mapper->addMapping(mpUidLabel, AccountDB::Constants::MP_UID);
    m_Mapper->addMapping(userUidLabel, AccountDB::Constants::MP_USER_UID);
    m_Mapper->addMapping(name, AccountDB::Constants::MP_NAME);
    m_Mapper->addMapping(mpComboBox, AccountDB::Constants::MP_NAME);
    m_Mapper->addMapping(abstractEdit, AccountDB::Constants::MP_ABSTRACT);
    m_Mapper->addMapping(type, AccountDB::Constants::MP_TYPE);
    m_Mapper->addMapping(amountSpin, AccountDB::Constants::MP_AMOUNT);
    m_Mapper->addMapping(rateSpin, AccountDB::Constants::MP_REIMBOURSEMENT);
    m_Mapper->addMapping(dateEdit, AccountDB::Constants::MP_DATE);

    mpComboBox->setModel(m_Model);
    mpComboBox->setModelColumn(AccountDB::Constants::MP_NAME);
    setDatasToUi();
}

MedicalProcedureWidget::~MedicalProcedureWidget()
{
    saveModel();
}

void MedicalProcedureWidget::setDatasToUi()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << "index row  =" << QString::number(mpComboBox->currentIndex());
    m_Mapper->setCurrentIndex(mpComboBox->currentIndex());
}

void MedicalProcedureWidget::saveModel()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());
    if (!m_Model->isDirty()) {qDebug() << __FILE__ << QString::number(__LINE__) << " is dirty ";
        bool yes = Utils::yesNoMessageBox(tr("Save changes ?"),
                                          tr("You make changes into the medical procedures table.\n"
                                             "Do you want to save them ?"));
        if (yes) {qDebug() << __FILE__ << QString::number(__LINE__) << " submit =";
            if (!m_Model->submit()) {
                Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).
                                                   arg(tr("medical_procedures")), __FILE__, __LINE__);
            }
        } else {
            m_Model->revert();
        }
//        qWarning() << __FILE__ << QString::number(__LINE__) << "error "<< m_Model->lastError().text() ;
    }
}

void MedicalProcedureWidget::on_mpComboBox_currentIndexChanged(int index)
{
    //saveModel();
    m_Mapper->setCurrentIndex(mpComboBox->currentIndex());
}

void MedicalProcedureWidget::on_addButton_clicked()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount1 =" << QString::number(m_Model->rowCount());
    if (!m_Model->insertRow(m_Model->rowCount()))
        Utils::Log::addError(this, "Unable to add row", __FILE__, __LINE__);
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount2 =" << QString::number(m_Model->rowCount());
    mpComboBox->setCurrentIndex(m_Model->rowCount()-1);
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setFocus();
    qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());
}

void MedicalProcedureWidget::on_save_clicked()
{
    saveModel();
}

//void MedicalProcedureWidget::on_name_textChanged(const QString & text){
//    mpComboBox->setItemText(mpComboBox->currentIndex(),text);
//}

void MedicalProcedureWidget::on_removeButton_clicked()
{
    if (!m_Model->removeRow(mpComboBox->currentIndex()))
    {
    	  Utils::Log::addError(this, "Unable to remove row", __FILE__, __LINE__);
        }
    mpComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}

void MedicalProcedureWidget::saveToSettings(Core::ISettings *sets)
{qDebug() << __FILE__ << QString::number(__LINE__) << " saveToSettings";
    if (!m_Model->submit()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("medical_procedures")));
        Utils::warningMessageBox(tr("Can not submit medical procedure to your personnal database."),
                                 tr("An error occured during medical procedures saving. Datas are corrupted."));
    }
        connect(name,SIGNAL(textEdited(const QString &)),mpComboBox,SLOT(setEditText(const QString &)));
        update();
}

void MedicalProcedureWidget::writeDefaultSettings(Core::ISettings *s)
{
//    Utils::Log::addMessage("MedicalProcedureWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("MedicalProcedureWidget"));
//    s->sync();
}

void MedicalProcedureWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
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
