#ifndef RECEIPTVIEWER_H
#define RECEIPTVIEWER_H

#include <QWidget>
#include <QStandardItemModel>
#include <QHash>
#include <QSqlError>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QStringListModel>
#include <QTreeView>

namespace Ui {
    class ReceiptViewer;
}

/** \todo this is not correct. Two choices: create its own files or hide it in the cpp in an anonymous namespace. */
namespace Internal {
class AmountModel;
}  // End namespace Internal


class treeViewsActions: public QTreeView
{
    Q_OBJECT

public:
    treeViewsActions(QWidget *parent);
    ~treeViewsActions();

    void reset() {QTreeView::reset();}
    void fillActionTreeView();
    QStandardItemModel *treeModel() const {return m_actionsTreeModel;}

private:
    void mousePressEvent(QMouseEvent *event);
    bool deleteItemFromThesaurus(QModelIndex &index);
    bool addPreferedItem(QModelIndex &index);
    bool isChildOfThesaurus();

private Q_SLOTS:
    void choosePreferedValue(bool b);
    void deleteBox(bool b);

private:
    /** \create a pimpl */
    QAction *m_choosePreferedValue;
    QAction *m_deleteThesaurusValue;
    QMenu *m_menuRightClic;
    QStandardItemModel *m_actionsTreeModel;
};


class ReceiptViewer : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiptViewer(QWidget *parent = 0);
    ~ReceiptViewer();
    void setPosition(QWidget *parent);

    /** \todo this is not recommended. Use const functions to get infos and define setters. */
    QString m_userUuid;
    treeViewsActions *actionTreeView;

protected:
    void changeEvent(QEvent *e);
    void fillModel(QHash<QString,QString> &hashOfValues, int typeOfPayment, double percentage);

    /** \todo remove this using the Core::ActionManager. */
    QAction *m_clear;
    QMenu *m_menu;

private:
    void mousePressEvent(QMouseEvent *event);
    QVariant firstItemChoosenAsPreferential(QString &item);

private Q_SLOTS:
    void deleteLine();
    void save();
    void saveAndQuit();
    void actionsOfTreeView(const QModelIndex &);
    void saveInThesaurus();

    /** \todo What does mean \e b ? Use explicit naming */
    void clearAll(bool b);

private:
    /** \todo create a pimpl */
    Ui::ReceiptViewer *ui;
    QStringListModel *m_modelReturnedList;
    Internal::AmountModel *m_model;
    QVariant m_siteUid;
    QVariant m_insuranceUid;
    double m_kilometers;
    double m_distanceRuleValue;
    QStringList m_listOfValues;
};

#endif // RECEIPTVIEWER_H
