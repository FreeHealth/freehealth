#ifndef USERIDENTITYANDLOGINPAGE_H
#define USERIDENTITYANDLOGINPAGE_H

#include <usermanagerplugin/iuserwizardpage.h>

#include <QWidget>

namespace Views {
class StringListView;
class LanguageComboBox;
}

namespace Utils {
class LineEditEchoSwitcher;
class UpperCaseValidator;
}

namespace UserPlugin {


namespace Internal {

namespace Ui {
class UserIdentityAndLoginPage;
}


    
class UserIdentityAndLoginPage: public QWizardPage
{
    Q_OBJECT

public:
    explicit UserIdentityAndLoginPage(QWidget *parent = 0);
    ~UserIdentityAndLoginPage();


    bool validatePage();

private Q_SLOTS:
    void checkLogin();
    void checkControlPassword(const QString &text);
    void on_leName_textChanged(const QString &text);
    void on_leFirstName_textChanged(const QString &text);
    void on_leLogin_textChanged(const QString &text);

private:
    void changeEvent(QEvent *e);
    void retranslate();
//    QLabel *langLbl, *lblTitle, *lblName, *lblFirstName, *lblSecondName, *lblGender, *lblLogin, *lblPassword, *lblConfirmPassword;
//    QComboBox *cbTitle, *cbGender;
//    QLineEdit *leName, *leFirstName, *leSecondName;
//    QGroupBox *identGroup, *logGroup;
//    Utils::LineEditEchoSwitcher  *lePassword, *lePasswordConfirm;
//    QLineEdit *leLogin;
    Internal::Ui::UserIdentityAndLoginPage *ui;
};

} // namespace Internal
} // namespace UserPlugin
#endif // USERIDENTITYANDLOGINPAGE_H
