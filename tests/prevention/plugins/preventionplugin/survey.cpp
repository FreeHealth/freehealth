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
     m_timer->start(200000);
}

//void Survey::iconsReset(const QHash<int,QVariant> &){}

void Survey::resetIcons()
{
    m_hashIconIdItem.clear();
    QSqlDatabase db = m_io->getDatabase();
    const QString warning = pixmaps()+"/preventWarning.png";
    const QString passed = pixmaps()+"/past.png";
    QSqlQuery q(db);
    QString req = QString("SELECT %1,%2 FROM %3 WHERE %4 < '%5' AND %6 NOT LIKE '%7' AND %8 NOT LIKE '%9'")
                        .arg("ICON",
                             "ID_ITEM",
                             "prevention",
                             "DATE_NEXT",
                             QDate::currentDate().toString("yyyy-MM-dd"),
                             "PARENT",
                             "ROOT",
                             "ICON",
                             warning
                             );
    req = req + QString(" AND %1 NOT LIKE '%2'").arg("ICON",passed);
    if (!q.exec(req))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
        }
    else{
        qDebug() << __FILE__ << QString::number(__LINE__) << " req =" << q.lastQuery() ;
        }
    while (q.next())
    {
    	int icon = q.value(0).toInt();
    	QVariant var = q.value(1);
    	qDebug() << __FILE__ << QString::number(__LINE__) << " varId =" << var.toString() ;
    	
    	m_hashIconIdItem.insertMulti(icon,var);
        }   
    emit iconsReset(m_hashIconIdItem);
}

QHash<int,QVariant> Survey::getHashIconDate()
{
    return m_hashIconIdItem;
}

