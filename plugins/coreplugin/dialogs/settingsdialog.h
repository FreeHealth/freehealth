#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QList>

namespace Core {
class IOptionsPage;

namespace Internal {
namespace Ui{
    class SettingsDialog;
} // Ui
} // Internal

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent,
                   const QString &initialCategory = QString(),
                   const QString &initialPage = QString());
    ~SettingsDialog();

    // Run the dialog and return true if 'Ok' was choosen or 'Apply' was invoked
    // at least once
    bool execDialog();

public slots:
    void done(int);

private slots:
    void pageSelected();
    void accept();
    void reject();
    void apply();

private:
    Internal::Ui::SettingsDialog *m_ui;
    QList<Core::IOptionsPage*> m_pages;
    bool m_applied;
    QString m_currentCategory;
    QString m_currentPage;
};

} // namespace Core

#endif // SETTINGSDIALOG_H
