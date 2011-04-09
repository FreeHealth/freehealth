#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QTextDocument>
#include <QMutex>
#include <QThread>
#include <QDate>
#include <QTextTableFormat>
#include <QTextCursor>
#include <QTextTableCell>

class ProduceDoc: public QThread {
    Q_OBJECT
    enum typesEnum {
        RECEIPTS_TYPE = 1,
        MOVEMENTS_TYPE
        };
    enum vectorReceipts {
        DATE = 0,
        PATIENT_NAME,
        CASH,
        CHECKS,
        VISA,
        BANKING,
        ACTS
        };
    enum sumsEnum {
        SUMS_SUM = 0,
        SUMS_CASH,
        SUMS_CHECKS,
        SUMS_CREDITCARDS,
        SUMS_BANKING,
        SUMS_MaxParam
        };
    enum vectorMovements {
        VM_DATE = 0,
        VM_LABEL,
        VM_AMOUNT,
        VM_COMMENT,
        VM_DETAILS
        };
    enum placeInVector {
        PLACE_OF_DATE = 0,
        PLACE_OF_PATIENT_NAME,
        PLACE_OF_CASH        
        };
    enum tableColumns {
        TABLE_DATE = 1,
        TABLE_NAME_OF_PATIENT,
        TABLE_SUMS,
        TABLE_NAME_OF_ACTS
        };
    public :
        ProduceDoc();
        ~ProduceDoc();
        int            m_fieldsToJoin;
        QString        m_tableFormatParameters;
        QString        m_tablesRecapParameters;
        QMutex         m_mutex;
        QStringList    m_typesReceipts;
        QStringList    m_typesMovements;
        QTextDocument* getTextDocument();
        void           dateChosen(QDate & date);
    private :
        QDate           m_date;
        QDate           m_date1;
        QTextDocument * m_textDocument;
        void        run();
        void        organize();
        void        fillTable(QList<QVector<QString> > & tableau,
                              QTextTableFormat & tableFormatOrganized,
                              QTextCursor * cursorForFillFunction,
                              QString & thisMonth, 
                              QStringList & listSums, 
                              int choice);
        QStringList calculateReceipts(QString & dateBegin, QString & dateEnd);
        QStringList calculateMovements(QString & dateBegin, QString & dateEnd);
    
                         
        QTextTableFormat myFormat(QTextTableFormat & tableFormat,QString & parametersForTableFormat);
                               
        bool modele(QString & month,
                    QString & day1 , 
                    QString & lastDay ,
                    QTextTableFormat & tableFormatOrganized,
                    QTextCursor *cursorForFunction);
    
    signals :
        void outThread(const QString & text);
        //void started();
    private slots :
        void recupSlot(const QString & text);
};

#endif
