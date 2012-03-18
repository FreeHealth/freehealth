#ifndef PREVENTIHM_H
#define PREVENTIHM_H

#include "preventIO.h"
#include "ui_prevent.h"
#include "moreIHM.h"
#include "survey.h"

#include <QWidget>
#include <QTreeView>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QSqlTableModel>



class TreeViewOfPrevention: public QTreeView
{
    Q_OBJECT

public:
    
    TreeViewOfPrevention(QWidget *parent = 0);
    ~TreeViewOfPrevention();

    void reset() {QTreeView::reset();}
    VariantItemModel * model();
    void getModel(VariantItemModel * model);


private:
    void mouseReleaseEvent(QMouseEvent *event);
    bool deleteItemAccordingToIndex(QModelIndex & index);
    bool isChild();



private Q_SLOTS:
    void deleteItem(bool);
    void showMore(bool);
    void addAValue(bool);
    //void userIsChanged(); TODO

private:
    //void changeEvent(QEvent *e); TODO

private:
    QAction *m_showMore;
    QAction *m_deleteValue;
    QAction *m_addValue;
    QMenu *m_menuRightClic;
    QString m_userUuid;
    
    VariantItemModel *m_model;

};

class PreventIHM : public QWidget, public Ui::PreventWidget {
    Q_OBJECT
    public :
        PreventIHM(QWidget * parent = 0);
        ~PreventIHM();
        VariantItemModel * m_modelOfItems;
        PreventIO *m_io;
        QSqlTableModel * m_model;
        QSqlTableModel * m_newModel;
    
    private slots :
        void iconsResetIfDateNextOvertaken(const QHash<int,QVariant>&);    
    private :
        QVBoxLayout *m_vbox;
        TreeViewOfPrevention *m_TreeViewOfPrevention; 
        Survey * m_survey;
        void changeIconWidget();
    

};

#endif

