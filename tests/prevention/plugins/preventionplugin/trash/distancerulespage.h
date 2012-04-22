
#ifndef DISTANCERULESPAGE_H
#define DISTANCERULESPAGE_H
#include "ui_distancerulespage.h"
#include <coreplugin/ioptionspage.h>
#include <accountbaseplugin/distancerulesmodel.h>

#include <QPointer>
#include <QString>
#include <QDataWidgetMapper>
#include <QHash>
#include <QSpinBox>
#include <QStandardItemModel>

namespace Core {
class ISettings;
}

namespace AccountDB {
class DistanceRulesModel;
}

namespace Account {
namespace Internal {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  DistanceRulesWidget  ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DistanceRulesWidget : public QWidget, private Ui::DistanceRulesWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DistanceRulesWidget)

public:
    explicit DistanceRulesWidget(QWidget *parent = 0);
    ~DistanceRulesWidget();
    void setDatasToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void on_distanceRulesComboBox_currentIndexChanged(int index);
    void on_addButton_clicked();
    void on_deleteButton_clicked();

private:
    void changeEvent(QEvent *e);
    void saveModel();
    QString calcDistanceRulesUid();
//    QStandardItemModel * distanceRulesModelByLocale();
//    bool fillEmptyDistanceRulesModel();

private:
    AccountDB::DistanceRulesModel *m_Model;
    QSpinBox *m_distancerulesUidLabel;
    QDataWidgetMapper *m_Mapper;
};

}  // End Internal


class DistanceRulesPage : public Core::IOptionsPage
{
public:
    DistanceRulesPage(QObject *parent = 0);
    ~DistanceRulesPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const {return name();}

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::DistanceRulesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::DistanceRulesWidget> m_Widget;
};

}  // End namespace Account

#endif

