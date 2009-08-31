#ifndef COMMONDEBUGPAGE_H
#define COMMONDEBUGPAGE_H

#include <coreplugin/idebugpage.h>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

namespace Core {
namespace Internal {

class LogMessageDebugPage : public IDebugPage
{
public:
    LogMessageDebugPage(QObject *parent);
    ~LogMessageDebugPage();

    QString id() const       { return objectName();   }
    QString name() const;
    QString category() const;

    void refreshContents();

    QWidget *widget();

private:
    QPointer<QWidget> m_Widget;
    QTreeWidget *tree;
};


class LogErrorDebugPage : public IDebugPage
{
public:
    LogErrorDebugPage(QObject *parent);
    ~LogErrorDebugPage();

    QString id() const       { return objectName();   }
    QString name() const;
    QString category() const;

    void refreshContents();

    QWidget *widget();

private:
    QPointer<QWidget> m_Widget;
    QTreeWidget *tree;
};


class SettingDebugPage : public IDebugPage
{
public:
    SettingDebugPage(QObject *parent);
    ~SettingDebugPage();

    QString id() const       { return objectName();   }
    QString name() const     { return tr("Settings"); }
    QString category() const { return tr("Settings"); }

    void refreshContents();

    QWidget *widget();

private:
    QPointer<QWidget> m_Widget;
    QTreeWidget *tree;
};

} // End Internal
} // End Core

#endif // COMMONDEBUGPAGE_H
