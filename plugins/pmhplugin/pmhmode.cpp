#include "pmhmode.h"
#include "pmhmodel.h"
#include "pmhcategorymodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include <listviewplugin/fancytreeview.h>
#include <listviewplugin/simplecategorymodel.h>

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

#include "ui_pmhmode.h"


using namespace PMH;
using namespace Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

PmhMode::PmhMode(QObject *parent) :
        Core::BaseMode(parent), ui(new Ui::PmhMode)
{
    setName(tr("Medical History"));
    setIcon(theme()->icon(Core::Constants::ICONPATIENTHISTORY, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_PATIENT_HISTORY);
    setUniqueModeName(Core::Constants::MODE_PATIENT_HISTORY);
//    const QList<int> &context;
//    setContext();

    ui->setupUi(this);
    Views::FancyTreeView *v = new Views::FancyTreeView;
//    v->setModel(new PmhModel(this), PmhModel::MH_EmptyColumn);
    v->setModel(new Views::SimpleCategoryModel("", this));
    v->setButtonActions(Views::FancyTreeView::FTV_SaveModel |  Views::FancyTreeView::FTV_CreateNew);
    v->useContextMenu(true);

    m_Widget = v;
    setWidget(m_Widget);

    // Get PMH categories

}

PmhMode::~PmhMode()
{
    delete ui;
}


