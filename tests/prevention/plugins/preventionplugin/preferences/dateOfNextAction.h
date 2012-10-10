#ifndef DATEOFNEXTACTION_H
#define DATEOFNEXTACTION_H

#include "ui_dateOfNextAction.h"
#include "../preventIO.h"

#include <coreplugin/ioptionspage.h>

#include <QPointer>
#include <QString>
#include <QDataWidgetMapper>
#include <QHash>
#include <QSpinBox>
#include <QSqlTableModel>
#include <QSqlDatabase>

namespace Core {
class ISettings;
}

namespace Prevention {
namespace Internal {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  NextAction  ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class NextAction : public QWidget, private Ui::NextDateWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(NextAction)
  enum NextDateColumns 
  {
      NEXTDATE_ITEM = 1,
      NEXTDATE_YEARS,
      NEXTDATE_MONTHS,
      NEXTDATE_DAYS,
      NEXTDATE_HOURS,
      NEXTDATE_MINUTES,
      NEXDATE_USER_UID,
      NextDateColumns_MaxParam
  };

  public:
    explicit NextAction(QWidget *parent = 0);
    ~NextAction();
    //static void writeDefaultSettings( Core::ISettings *s );

  public Q_SLOTS:
    void addRow();
    void deleteRow();
    void listComboBoxIndexChanged(int);
    void saveInModel();
    void revertChangesAndClose();
  private:
    QSqlDatabase m_db;
    //void changeEvent(QEvent *e);

private:
    PreventIO *m_io;
    QDataWidgetMapper *m_Mapper;
    QSqlTableModel * m_model;
};

}  // End Internal

}  // End Prevention

#endif


