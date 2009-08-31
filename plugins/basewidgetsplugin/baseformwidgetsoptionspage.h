#ifndef BASEFORMWIDGETSOPTIONSPAGE_H
#define BASEFORMWIDGETSOPTIONSPAGE_H

#include <coreplugin/ioptionspage.h>
#include <QtCore/QObject>
#include <QtGui/QWidget>

#include <QPointer>

namespace BaseWidgets {
namespace Internal {

namespace Ui {
    class BaseFormWidgetsOptionsPage;
}

class BaseFormSettingsWidget : public QWidget
{
public:
    BaseFormSettingsWidget(QWidget *parent = 0);
    ~BaseFormSettingsWidget();

    void applyChanges();
    void resetToDefaults();

private:
    Ui::BaseFormWidgetsOptionsPage *m_ui;
};

class BaseFormWidgetsOptionsPage : public Core::IOptionsPage
{
public:
    BaseFormWidgetsOptionsPage(QObject *parent = 0);
    ~BaseFormWidgetsOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void applyChanges();
    void finish();

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<BaseFormSettingsWidget> m_Widget;
};

} // End Internal
} // End BaseWidgets

#endif // BASEFORMWIDGETSOPTIONSPAGE_H
