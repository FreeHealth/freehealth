#include "survey.h"

static inline QString pixmaps() 
{ 
    return QString(qApp->applicationDirPath()+"/../../global_resources/pixmap");
};

Survey::Survey(QObject * parent)
{
    m_io = new PreventIO(this);
    
    setTimer();
}

Survey::~Survey(){}

void Survey::setTimer()
{
     m_timer = new QTimer(this);
     connect(m_timer, SIGNAL(timeout()), this, SLOT(resetIcons()));
     m_timer->start(2000);
}

//void Survey::iconsReset(const QHash<int,QVariant> &){}

void Survey::resetIcons()
{
    m_hashIconIdItem.clear();
    QSqlDatabase db = m_io->getDatabase();
    QSqlQuery q(db);
    const QString req = QString("SELECT %1,%2 FROM %3 WHERE %4 < '%5'")
                        .arg("ICON",
                             "ID_ITEM",
                             "prevention",
                             "DATE_NEXT",
                             QDate::currentDate().toString("yyyy-MM-dd"));
    if (!q.exec(req))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
        }
    
    while (q.next())
    {
    	int icon = q.value(0).toInt();
    	QVariant var = q.value(1);
    	m_hashIconIdItem.insertMulti(icon,var);
        }   
    emit iconsReset(m_hashIconIdItem);
}

QHash<int,QVariant> Survey::getHashIconDate()
{
    return m_hashIconIdItem;
}

