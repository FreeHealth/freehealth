#ifndef MOREIHM_H
#define MOREIHM_H

#include "preventIO.h"
#include "preventIHM.h"

#include "ui_moreIHM.h"
#include <QWidget>
#include <QMouseEvent>
#include <QSqlDatabase>
#include <QHash>

class TreeViewOfPrevention;

class MoreIHM : public QWidget , Ui::MoreWidget{
    Q_OBJECT
    enum DataAndId
    {
        DATA = 0,
        ID
    };
    public:
        MoreIHM(QObject *parent, TreeViewOfPrevention * treeView , QModelIndex index = QModelIndex());
        ~MoreIHM();
    
    private Q_SLOTS:
        void showDocumentAccordingToComboChoice(int);
        void closeAndQuit();
        void saveTextBrowser();
    private:
        void closeMoreWidget();
        //void mouseReleaseEvent(QMouseEvent *event);
        PreventIO *m_io;
        QSqlDatabase m_db;
        QMap<int,QVector<QVariant> > m_hashItemAndId;
        QHash<int,QString> m_hashIndexAndId;
};

#endif

