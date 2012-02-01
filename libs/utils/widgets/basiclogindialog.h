#ifndef UTILS_BASICLOGINDIALOG_H
#define UTILS_BASICLOGINDIALOG_H

#include <QDialog>

namespace Utils {

namespace Ui {
    class BasicLoginDialog;
}

class BasicLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BasicLoginDialog(QWidget *parent = 0);
    ~BasicLoginDialog();

    void setTitle(const QString &title);

    void setToggleViewIcon(const QString &fullAbsPath);

    QString login() const;
    QString password() const;

private:
    Ui::BasicLoginDialog *ui;
};


} // namespace Utils
#endif // UTILS_BASICLOGINDIALOG_H
