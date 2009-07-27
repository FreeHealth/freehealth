#ifndef TKCONTEXT_H
#define TKCONTEXT_H

#include <tkExporter.h>

#include <QObject>
#include <QWidget>

class Q_TK_EXPORT tkContext : public QObject
{
public:
    tkContext(QObject *parent = 0) : QObject(parent) {}
    virtual ~tkContext() {}

    virtual QList<int> context() const = 0;
    virtual QWidget *widget() = 0;
    virtual QString contextHelpId() const { return QString(); }
};


#endif // TKCONTEXT_H
