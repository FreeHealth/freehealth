#include "pmhepisodeviewer.h"
#include "pmhepisodemodel.h"
#include "pmhdata.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include <icdplugin/icdcollectiondialog.h>

#include <utils/widgets/datetimedelegate.h>
#include <utils/log.h>

#include <QDialog>


#include "ui_pmhepisodeviewer.h"

using namespace PMH;
using namespace Internal;


static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }


namespace PMH {
namespace Internal {
class PmhEpisodeViewerPrivate
{
public:
    PmhEpisodeViewerPrivate() : m_Pmh(0) {}

public:
    PmhData *m_Pmh;
};
}  // End namespace Internal
}  // End namespace PMH

/** \todo add Contacts */
PmhEpisodeViewer::PmhEpisodeViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PmhEpisodeViewer),
    d(new Internal::PmhEpisodeViewerPrivate)
{
    ui->setupUi(this);

    // Create date delegates
    Utils::DateTimeDelegate *start = new Utils::DateTimeDelegate(this, true);
    Utils::DateTimeDelegate *end = new Utils::DateTimeDelegate(this, true);
    QDate birth = patient()->data(Core::IPatient::DateOfBirth).toDate();
    if (birth.isValid()) {
        start->setDateRange(birth, birth.addYears(150));
        end->setDateRange(birth, birth.addYears(150));
    }
    ui->tableView->setItemDelegateForColumn(PmhEpisodeModel::DateStart, start);
    ui->tableView->setItemDelegateForColumn(PmhEpisodeModel::DateEnd, end);

    // ICD10 coding
    connect(ui->tableView, SIGNAL(activated(QModelIndex)), this, SLOT(itemActivated(QModelIndex)));
}

PmhEpisodeViewer::~PmhEpisodeViewer()
{
    delete ui;
    if (d)
        delete d;
    d = 0;
}

void PmhEpisodeViewer::setPmhData(Internal::PmhData *pmh)
{
    Q_ASSERT(pmh);
    if (!pmh)
        return;
    d->m_Pmh = pmh;
    ui->tableView->setModel(pmh->episodeModel());
    ui->tableView->hideColumn(PmhEpisodeModel::IcdXml);
    ui->tableView->hideColumn(PmhEpisodeModel::Contact);
    ui->tableView->hideColumn(PmhEpisodeModel::EmptyColumn);
    ui->tableView->horizontalHeader()->setResizeMode(PmhEpisodeModel::Label, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setResizeMode(PmhEpisodeModel::DateEnd, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setResizeMode(PmhEpisodeModel::DateStart, QHeaderView::Stretch);
}

void PmhEpisodeViewer::itemActivated(const QModelIndex &item)
{
    if (item.column()==PmhEpisodeModel::IcdCodeList) {
        // create an ICD10 collection dialog
        ICD::IcdCollectionDialog dlg(this);
        // get the XML ICD10 coding
        QString xml = item.model()->index(item.row(), PmhEpisodeModel::IcdXml).data(Qt::EditRole).toString();
        LOG(this, xml);
        dlg.setXmlIcdCollection(xml);
        if (dlg.exec()==QDialog::Accepted) {
            // retrieve selected codes to the PmhEpisodeModel
            d->m_Pmh->episodeModel()->setData(item.model()->index(item.row(), PmhEpisodeModel::IcdXml), dlg.xmlIcdCollection());
        }
    }
}

void PmhEpisodeViewer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
