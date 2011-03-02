#ifndef ACCOUNTCONTEXTUALWIDGET_H
#define ACCOUNTCONTEXTUALWIDGET_H

#include <accountplugin/account_exporter.h>

#include <QWidget>

/**
 * \file accountcontextualwidget.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.1.0
 * \date 03 Mar 2011
 * \internal
*/

namespace Account {
namespace Internal {
class AccountContext;
}

class ACCOUNT_EXPORT AccountContextualWidget : public QWidget
{
    Q_OBJECT
public:
    AccountContextualWidget(QWidget *parent = 0);
    virtual ~AccountContextualWidget();

    void addContexts(const QList<int> &contexts);
    QList<int> contexts() const;

private:
    Internal::AccountContext *m_Context;
};

}  // End namespace Account


#endif // ACCOUNTCONTEXTUALWIDGET_H
