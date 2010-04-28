#ifndef FORMPLACEHOLDER_H
#define FORMPLACEHOLDER_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <QWidget>
QT_BEGIN_NAMESPACE
class QTreeWidget;
class  QTreeWidgetItem;
class QStackedLayout;
QT_END_NAMESPACE

namespace Form {
namespace Internal {
class FormPlaceHolderPrivate;
}  // End namespace Internal

class FORM_EXPORT FormPlaceHolder : public QWidget
{
    Q_OBJECT

public:
    FormPlaceHolder(QWidget *parent = 0);
    ~FormPlaceHolder();

    QTreeWidget *formTree() const;
    QStackedLayout *formStackLayout() const;
    void clearFormStackLayout();
    
    void addTopWidget(QWidget *top);
    void addBottomWidget(QWidget *bottom);

protected Q_SLOTS:
    void changeStackedLayoutTo(QTreeWidgetItem *item);

private:
    Internal::FormPlaceHolderPrivate *d;
};

}  // End namespace Form

#endif // FORMPLACEHOLDER_H
