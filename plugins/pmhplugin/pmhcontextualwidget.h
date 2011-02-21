#ifndef PMHCONTEXTUALWIDGET_H
#define PMHCONTEXTUALWIDGET_H

#include <QWidget>

/**
 * \file pmhcontextualwidget.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 12 Feb 2011
 * \internal
*/


namespace PMH {
namespace Internal {
class PmhContext;
}

class PmhContextualWidget : public QWidget
{
    Q_OBJECT
public:
    PmhContextualWidget(QWidget *parent = 0);
    virtual ~PmhContextualWidget();

    void addContexts(const QList<int> &contexts);
    QList<int> contexts() const;

private:
    Internal::PmhContext *m_Context;
};

}  // End namespace PMH

#endif // PMHCONTEXTUALWIDGET_H
