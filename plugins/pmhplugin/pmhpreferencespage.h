#ifndef PMHPREFERENCESPAGE_H
#define PMHPREFERENCESPAGE_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>

#include "ui_pmhpreferencespage.h"

/**
 * \file pmhpreferencespages.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 18 Feb 2011
*/
namespace Core {
class ISettings;
}


namespace PMH {
namespace Internal {

class PmhPreferencesWidget : public QWidget, private Ui::PmhPreferencesWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(PmhPreferencesWidget)

public:
    explicit PmhPreferencesWidget(QWidget *parent = 0);
    void setDatasToUi();

    static void writeDefaultSettings(Core::ISettings *s);
    static void applyToModel();

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

private Q_SLOTS:
    void on_changePmhFont_clicked();
    void on_changeCatFont_clicked();

protected:
    virtual void changeEvent(QEvent *e);
};

class PmhPreferencesPage : public Core::IOptionsPage
{
public:
    PmhPreferencesPage(QObject *parent = 0);
    ~PmhPreferencesPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return "parametrer.html";}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::PmhPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::PmhPreferencesWidget> m_Widget;
};


}
}

#endif // PMHPREFERENCESPAGE_H
