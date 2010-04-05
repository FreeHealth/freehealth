#include "TestWidget.h"

#include <tkActionManager.h>

#include <tkConstants.h>
#include <tkContextManager.h>


#define GET_AM(dd)  tkActionManager *am = tkActionManager::instance( dd );
#define GET_CM(dd)  tkContextManager *cm = tkContextManager::instance( dd );


TestWidget::TestWidget( QWidget *parent ) : QWidget(parent)
{
    static int handler = 0;
    handler++;
    setObjectName("TestWidget"+QString::number(handler));
    qWarning() << "created" << objectName();
    QGridLayout l(this);
    line = new QLineEdit(this);
    line->setText( "TestWidget" + QString::number(handler+12) );
    line->setObjectName( objectName() + QString::number(handler) );
    l.addWidget(line, 0,0);
    setLayout(&l);
    setMinimumSize(22,30);
    setMaximumSize(32767, 32767);
}

void TestWidget::init()
{
    static int handler = 0;
    handler++;
    QList<int> c;
    c << handler+12;
    GET_AM();
    QAction *m_openAction = new QAction(QIcon("Constants::ICON_OPENFILE_2"), tr("&Open dialog"), this);
    tkCommand *cmd = am->registerAction(m_openAction, "Constants::OPEN_DIALOG", c);
    cmd->setDefaultKeySequence(QKeySequence::Open);
    am->actionContainer(tkConstants::M_FILE)->addAction(cmd, "Constants::G_FILE_OPEN");
    connect(m_openAction, SIGNAL(triggered()), this, SLOT(openDialog()));

    // Surcharger une action d'un menu == créer une nouvelle action et lui donner le même ID, context différent
    QAction *m_newAction = new QAction(QIcon("Constants::ICON_NEWFILE"), tr("&New..."), this);
    am->registerAction(m_newAction, "tkConstants::NEW", c);
    // Connecter l'action crée
    connect(m_newAction,SIGNAL(triggered()), this, SLOT(openDialog()));

    GET_CM();
    cm->addContextObject(new TestWidgetContext(this,c));
}

void TestWidget::openDialog()
{
    QDialog dlg;
    QGridLayout l(&dlg);
    QTextEdit t(&dlg);
    l.addWidget(&t, 0,0);
    QAction *a = static_cast<QAction*>(sender());
    t.setText( objectName() + " creates the dialog . Sender = " + a->text()  );
    dlg.exec();
}
