#ifndef PREVENTIHM_H
#define PREVENTIHM_H

#include "preventIO.h"
#include "ui_prevent.h"
#include "moreIHM.h"
#include "survey.h"
#include "treeviewdelegate.h"

#include <QWidget>
#include <QTreeView>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QSplitter>
#include <QSqlTableModel>
#include <QTextEdit>

class TreeViewOfPrevention: public QTreeView
{
    Q_OBJECT

public:
    
    TreeViewOfPrevention(QObject *parent = 0);
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
    void addAGroup(bool);
    void deleteGroup(bool);
    void addPreventionPreferences(bool);
    //void userIsChanged(); TODO

private:
    //void changeEvent(QEvent *e); TODO

private:
    QAction *m_showMore;
    QAction *m_deleteValue;
    QAction *m_addValue;
    QAction *m_addGroup;
    QAction *m_deleteGroup;
    QAction *m_defineNextDate;
    QMenu *m_menuRightClic;
    QString m_userUuid;
    VariantItemModel *m_model;
    PreventIO *m_io;
    QObject * m_parent;
};

class PreventIHM : public QWidget, public Ui::PreventWidget {
    Q_OBJECT
    public :
        PreventIHM(QWidget * parent = 0);
        ~PreventIHM();
        VariantItemModel * m_modelOfItems;
        
    public Q_SLOTS :
        void changeIconWidget();
    private Q_SLOTS :
        void iconsResetIfDateNextOvertaken(const QHash<int,QVariant>&);   
        void showDocument(const QModelIndex &);
        
    private :
        PreventIO *m_io;
        QVBoxLayout *m_vbox;
        QSplitter *m_vboxSplitter;
        QTextEdit *m_Editor;
        TreeViewOfPrevention *m_TreeViewOfPrevention; 
        Survey * m_survey;
};

#endif

