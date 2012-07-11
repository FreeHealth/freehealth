#include "dateOfNextAction.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/database.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>

#include <coreplugin/constants_icons.h>

enum { WarnDebugMessage = false };

using namespace Prevention;
using namespace Prevention::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }

NextAction::NextAction(QWidget * parent) :
    QWidget(parent)
{
    setObjectName("NextDateWidget");
    setupUi(this);
    addButton->setText("");
    deleteButton->setText("");    
    addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    deleteButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    m_io = new PreventIO(this);
    QString userUuid = m_io->getUserUid();  
    m_model = m_io->getNextDateModel();
    userUidLabel->setText(userUuid);
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setModel(m_model);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->addMapping(userUidLabel,NEXDATE_USER_UID);
    m_Mapper->addMapping(lineEdit,NEXTDATE_ITEM);
    m_Mapper->addMapping(yearSpinBox,NEXTDATE_YEARS);
    m_Mapper->addMapping(monthSpinBox,NEXTDATE_MONTHS);
    m_Mapper->addMapping(daySpinBox,NEXTDATE_DAYS);
    m_Mapper->addMapping(hourSpinBox,NEXTDATE_HOURS);
    m_Mapper->addMapping(minuteSpinBox,NEXTDATE_MINUTES);
    m_Mapper->toFirst();
    listComboBox->setModel(m_model);
    listComboBox->setModelColumn(NEXTDATE_ITEM);
    m_Mapper->setCurrentIndex(listComboBox->currentIndex());
    connect(addButton,SIGNAL(pressed()),this,SLOT(addRow()));
    connect(deleteButton,SIGNAL(pressed()),this,SLOT(deleteRow()));
    connect(listComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(listComboBoxIndexChanged(int)));
    connect(applyButton,SIGNAL(pressed()),this,SLOT(saveInModel()));
    connect(okButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(cancelButton,SIGNAL(pressed()),this,SLOT(revertChangesAndClose()));
}

NextAction::~NextAction(){}

void NextAction::addRow()
{
    m_model->insertRows(m_model->rowCount(),1,QModelIndex());
    userUidLabel->setFocus();
    listComboBox->setCurrentIndex(m_model->rowCount()-1);
}

void NextAction::deleteRow()
{
    m_model->removeRows(listComboBox->currentIndex(),1,QModelIndex());
    listComboBox->setCurrentIndex(m_model->rowCount() - 1);
}

void NextAction::listComboBoxIndexChanged(int index)
{
    m_Mapper->setCurrentIndex(index);
}

void NextAction::saveInModel()
{
    if (!m_model->submit()) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "unable to save changes in table nextdate" ;
       }
}

void NextAction::revertChangesAndClose()
{
    m_model->revert();
}

