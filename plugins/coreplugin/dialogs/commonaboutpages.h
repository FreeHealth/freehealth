#ifndef COMMONABOUTPAGES_H
#define COMMONABOUTPAGES_H

#include <coreplugin/iaboutpage.h>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QLabel;
class QTreeWidget;
class QTextBrowser;
QT_END_NAMESPACE

namespace Core {
namespace Internal {

class BuildAboutPage : public IAboutPage
{
public:
    BuildAboutPage(QObject *parent);
    ~BuildAboutPage();

    QString id() const       { return objectName();   }
    QString name() const     { return tr("4. Build informations"); }
    QString category() const { return tr("Application"); }

    void refreshContents();

    QWidget *widget();

private:
    QPointer<QWidget> m_Widget;
    QTreeWidget *tree;
};

class LicenseAboutPage : public IAboutPage
{
public:
    LicenseAboutPage(QObject *parent);
    ~LicenseAboutPage();

    QString id() const       { return objectName();   }
    QString name() const     { return tr("3. License"); }
    QString category() const { return tr("Application"); }

    void refreshContents();

    QWidget *widget();

private:
    QPointer<QWidget> m_Widget;
    QTextBrowser *browser;
};

class TeamAboutPage : public IAboutPage
{
public:
    TeamAboutPage(QObject *parent);
    ~TeamAboutPage();

    QString id() const       { return objectName();   }
    QString name() const     { return tr("2. Team"); }
    QString category() const { return tr("Application"); }

    void refreshContents();

    QWidget *widget();

private:
    QPointer<QWidget> m_Widget;
    QTreeWidget *tree;
};



} // End Internal
} // End Core

#endif // COMMONABOUTPAGES_H
