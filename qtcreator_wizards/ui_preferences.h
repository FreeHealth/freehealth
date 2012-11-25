/********************************************************************************
** Form generated from reading UI file 'preferences.ui'
**
** Created: Sun Nov 25 23:33:39 2012
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCES_H
#define UI_PREFERENCES_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

namespace %PluginName:c% {
namespace Internal {

class Ui_%PluginName:c%PreferencesWidget
{
public:

    void setupUi(QWidget *_PluginName_c___Internal___PluginName_c_PreferencesWidget)
    {
        if (_PluginName_c___Internal___PluginName_c_PreferencesWidget->objectName().isEmpty())
            _PluginName_c___Internal___PluginName_c_PreferencesWidget->setObjectName(QString::fromUtf8("_PluginName_c___Internal___PluginName_c_PreferencesWidget"));
        _PluginName_c___Internal___PluginName_c_PreferencesWidget->resize(400, 300);

        retranslateUi(_PluginName_c___Internal___PluginName_c_PreferencesWidget);

        QMetaObject::connectSlotsByName(_PluginName_c___Internal___PluginName_c_PreferencesWidget);
    } // setupUi

    void retranslateUi(QWidget *_PluginName_c___Internal___PluginName_c_PreferencesWidget)
    {
        _PluginName_c___Internal___PluginName_c_PreferencesWidget->setWindowTitle(QApplication::translate("%PluginName:c%::Internal::%PluginName:c%PreferencesWidget", "%PluginName:c% Preferences", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

} // namespace Internal
} // namespace %PluginName:c%

namespace %PluginName:c% {
namespace Internal {
namespace Ui {
    class %PluginName:c%PreferencesWidget: public Ui_%PluginName:c%PreferencesWidget {};
} // namespace Ui
} // namespace Internal
} // namespace %PluginName:c%

#endif // UI_PREFERENCES_H
