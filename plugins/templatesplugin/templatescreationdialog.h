#ifndef TEMPLATESCREATIONDIALOG_H
#define TEMPLATESCREATIONDIALOG_H

#include <templatesplugin/templates_exporter.h>

#include <QDialog>

namespace Templates {
namespace Internal {
namespace Ui {
    class TemplatesCreationDialog;
}  // End namespace Ui
}  // End namespace Internal

class TEMPLATES_EXPORT TemplatesCreationDialog : public QDialog
{
    Q_OBJECT
public:
    TemplatesCreationDialog(QWidget *parent = 0);
    ~TemplatesCreationDialog();

    void setTemplateContent(const QString &content) {m_Content = content;}
    void setTemplateSummary(const QString &summary);

protected:
    void done(int r);
    void changeEvent(QEvent *e);

private:
    Internal::Ui::TemplatesCreationDialog *ui;
    QString m_Content;
};

}  // End namespace Templates

#endif // TEMPLATESCREATIONDIALOG_H
