#ifndef TKCONTEXTMANAGER_H
#define TKCONTEXTMANAGER_H

#include <tkExporter.h>

#include <QObject>
#include <QWidget>
#include <QMainWindow>

#include <tkContext.h>

class tkContextManagerPrivate;

class Q_TK_EXPORT tkContextManager : public QObject
{
    Q_OBJECT
public:
    static tkContextManager *instance(QMainWindow *mainWin = 0);

    tkContext *contextObject(QWidget *widget);
    void addContextObject(tkContext *context);
    void removeContextObject(tkContext *context);
    void resetContext();

//    int getNewUniqueContext();

    tkContext * currentContextObject() const;
    void addAdditionalContext(int context);
    void removeAdditionalContext(int context);
    bool hasContext(int context) const;

    void updateContext();

Q_SIGNALS:
    void contextAboutToChange(tkContext *context);
    void contextChanged(tkContext *context);

private Q_SLOTS:
    void updateFocusWidget(QWidget *old, QWidget *now);

private:
    tkContextManager(QMainWindow *mainWin);
    void updateContextObject(tkContext *context);

private:
    tkContextManagerPrivate *d;
    static tkContextManager *m_Instance;
};

#endif // TKCONTEXTMANAGER_H
