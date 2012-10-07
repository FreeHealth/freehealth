#include "fullreleasepage.h"

#include <coreplugin/constants_icons.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

#include "ui_fullreleasepage.h"

#include <QLabel>
#include <QMovie>
#include <QTimer>

using namespace Core;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

FullReleasePage::FullReleasePage(QObject *parent) :
        IToolPage(parent), m_Widget(0), m_CentralWidget(0), ui(0), m_Spacer(0)
{
}

FullReleasePage::~FullReleasePage()
{
    if (ui)
        delete ui; ui=0;
    if (m_Spacer)
        delete m_Spacer; m_Spacer = 0;
}

QIcon FullReleasePage::icon() const
{
    return theme()->icon(Constants::ICONPROCESS, ITheme::MediumIcon);
}

QWidget *FullReleasePage::createPage(QWidget *parent)
{
    m_Widget = new QWidget(parent);
    QWidget *container = new QWidget;
    QVBoxLayout *lcont = new QVBoxLayout(container);
    lcont->setMargin(0);
    lcont->setSpacing(0);
    container->setLayout(lcont);

    m_CentralWidget = new QWidget(m_Widget);
    QVBoxLayout *l = new QVBoxLayout(m_CentralWidget);
    m_CentralWidget->setLayout(l);

    lcont->addWidget(m_CentralWidget);
//    lcont->addSpacerItem(m_Spacer = new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::Expanding));

    ui = new Ui::FullReleasePage;
    ui->setupUi(m_Widget);
    ui->scrollArea->setWidget(container);
    return m_Widget;
}

void FullReleasePage::addDownloadingProcess(const QString &message, const QString &id)
{
    QWidget *w = new QWidget(m_Widget);
    QHBoxLayout *l = new QHBoxLayout(w);
    w->setLayout(l);

    QLabel *lblMovie = new QLabel(w);
    lblMovie->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QMovie *movie = new QMovie(theme()->iconFullPath(Core::Constants::ICONSPINNER));
    lblMovie->setMovie(movie);
    movie->start();
    l->addWidget(lblMovie);
    m_IconLabels.insert(id, lblMovie);

    QLabel *lbl = new QLabel(w);
    lbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    lbl->setText(message + "... " + tr("Downloading") + "...");
    l->addWidget(lbl);

    l->addSpacerItem(new QSpacerItem(10,10, QSizePolicy::Expanding));

    m_CentralWidget->layout()->addWidget(w);
    //qApp->processEvents();
}

void FullReleasePage::endDownloadingProcess(const QString &id)
{
    // Remove movie
    QLabel *lbl = m_IconLabels.value(id, 0);
    if (lbl) {
        QMovie *movie = lbl->movie();
        delete movie;
        movie = 0;
        lbl->setMovie(0);
        lbl->setPixmap(theme()->icon(Core::Constants::ICONOK).pixmap(16,16));
    }
    //qApp->processEvents();
}

void FullReleasePage::addRunningProcess(const QString &message)
{
    QWidget *w = new QWidget(m_Widget);
    QHBoxLayout *l = new QHBoxLayout(w);
    w->setLayout(l);

    QLabel *lblMovie = new QLabel(w);
    lblMovie->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QMovie *movie = new QMovie(theme()->iconFullPath(Core::Constants::ICONSPINNER));
    lblMovie->setMovie(movie);
    movie->start();
    l->addWidget(lblMovie);
    m_IconLabels.insert("__processRunning__", lblMovie);

    QLabel *lbl = new QLabel(w);
    lbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    lbl->setText(message + "... " + tr("Processing") + "...");
    l->addWidget(lbl);

    l->addSpacerItem(new QSpacerItem(10,10, QSizePolicy::Expanding));

    m_CentralWidget->layout()->addWidget(w);
    //qApp->processEvents();
}

void FullReleasePage::endLastAddedProcess()
{
    // Remove movie
    QLabel *lbl = m_IconLabels.value("__processRunning__", 0);
    if (lbl) {
        QMovie *movie = lbl->movie();
        delete movie;
        movie = 0;
        lbl->setMovie(0);
        lbl->setPixmap(theme()->icon(Core::Constants::ICONOK).pixmap(16,16));
    }
    //qApp->processEvents();
}
