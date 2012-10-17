#include "addremovecombobox.h"

#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_icons.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>
#include <translationutils/constants.h>

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStylePainter>
#include <QDebug>

using namespace Views;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

AddRemoveComboBox::AddRemoveComboBox(QWidget *parent) :
    QWidget(parent)
{
    mLabel = new QLabel("");
    initialize();
}

AddRemoveComboBox::AddRemoveComboBox(const QString &labelText, QGroupBox *box, QWidget *parent) :
    QWidget(parent)
{
    mLabel = new QLabel(labelText);
    initialize();
}

void AddRemoveComboBox::initialize()
{
    QHBoxLayout *layout = new QHBoxLayout();

    mCombo = new QComboBox(this);
    mCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mLabel->setBuddy(mCombo);

    mAddButton = new QPushButton(this);
    mAddButton->setMinimumSize(22, 22);
    mAddButton->setMaximumSize(22, 22);
    mAddButton->setIcon(theme()->icon(Core::Constants::ICONADD));

    mRemoveButton = new QPushButton(this);
    mRemoveButton->setMinimumSize(22, 22);
    mRemoveButton->setMaximumSize(22, 22);
    mRemoveButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    mRemoveButton->setEnabled(false);

    layout->addWidget(mLabel);
    layout->addWidget(mCombo);
    layout->addWidget(mAddButton);
    layout->addWidget(mRemoveButton);
    setLayout(layout);

    connect(mAddButton, SIGNAL(clicked()), this, SLOT(addItem()));
    connect(mRemoveButton, SIGNAL(clicked()), this, SLOT(removeItem()));
    connect(mCombo, SIGNAL(currentIndexChanged(int)), this, SIGNAL(currentIndexChanged(int)));
}

AddRemoveComboBox::~AddRemoveComboBox()
{
}

void AddRemoveComboBox::setLabel(const QString &text)
{
    Q_ASSERT(mLabel);
    mLabel->setText(text);
}

void AddRemoveComboBox::addItem()
{
    QAbstractItemModel *model = mCombo->model();
    if (!model)
        return;
    // insert an empty row
    if(!model->insertRow(model->rowCount()))
        LOG_ERROR("Unable to add row " + QString::number(mCombo->currentIndex()));

    // set the ComboBox to that row
    mCombo->setCurrentIndex(model->rowCount()-1);

    //inform other widgets of successfully added item
    Q_EMIT itemAdded(model->index(model->rowCount()-1, 0));
}

void AddRemoveComboBox::removeItem()
{
    QAbstractItemModel *model = mCombo->model();
    if (!model->removeRow(mCombo->currentIndex())) {
          LOG_ERROR("Unable to remove row " + QString::number(mCombo->currentIndex()));
    }
    const bool enabled = (mCombo->count() > 0);
    mCombo->setCurrentIndex(mCombo->count() - 1);
    mRemoveButton->setEnabled(enabled);

    Q_EMIT itemRemoved();
}

void AddRemoveComboBox::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        mAddButton->setToolTip(tkTr(Trans::Constants::FILENEW_TEXT));
        mRemoveButton->setToolTip(tkTr(Trans::Constants::REMOVE_TEXT));
        break;
    default:
        break;
    }
}

void AddRemoveComboBox::setEditText(const QString &text)
{
    mCombo->setEditText(text);
}

