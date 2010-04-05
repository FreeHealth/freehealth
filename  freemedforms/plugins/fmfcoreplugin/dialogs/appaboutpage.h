#ifndef APPABOUTPAGE_H
#define APPABOUTPAGE_H

#include <coreplugin/iaboutpage.h>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace Core {
namespace Internal {

class AppAboutPage : public IAboutPage
{
public:
    AppAboutPage(QObject *parent);
    ~AppAboutPage();

    QString id() const       { return objectName();   }
    QString name() const     { return tr("1. General"); }
    QString category() const { return tr("Application"); }

    void refreshContents();

    QWidget *widget();

private:
    QPointer<QWidget> m_Widget;
    QLabel *label;
};

} // End Internal
} // End Core

#endif // APPABOUTPAGE_H
