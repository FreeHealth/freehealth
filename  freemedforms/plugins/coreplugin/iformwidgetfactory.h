#ifndef IFORMWIDGETFACTORY_H
#define IFORMWIDGETFACTORY_H

#include <coreplugin/core_exporter.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QWidget>
#include <QtGui/QFrame>
#include <QtGui/QBoxLayout>
#include <QtGui/QLabel>

namespace Core {

class IFormWidget;
class FormItem;

// This part is very closed to QDesignerCustomWidgetInterface so the translation should be easier
class CORE_EXPORT IFormWidgetFactory : public QObject
{
    Q_OBJECT
public:
    IFormWidgetFactory(QObject *parent = 0) : QObject(parent) {}
    virtual ~IFormWidgetFactory() {}

    virtual bool initialize(const QStringList &arguments, QString *errorString) = 0;
    virtual bool extensionInitialized() = 0;
    virtual QStringList providedWidgets() const = 0;
    virtual bool isContainer( const int idInStringList ) const = 0;
    inline bool isContainer(const QString &name) {return isContainer(providedWidgets().indexOf(name)); }
    virtual bool isInitialized() const = 0;
    virtual IFormWidget *createWidget(const QString &name, Core::FormItem *linkedObject, QWidget *parent = 0) = 0;
};


class CORE_EXPORT IFormWidget : public QWidget
{
    Q_OBJECT
public:
    enum QFrameStyleForLabels {
        FormLabelFrame = QFrame::Panel | QFrame::Sunken,
        ItemLabelFrame = QFrame::NoFrame,
        HelpTextFrame =  QFrame::Panel | QFrame::Sunken
    };

    enum LabelOptions {
        Label_NoOptions = 0,
        Label_NoLabel,
        Label_OnTop,
        Label_OnBottom,
        Label_OnLeft,
        Label_OnRigth,
        Label_OnTopCentered
    };

    IFormWidget(Core::FormItem *linkedObject, QWidget *parent = 0);
    virtual ~IFormWidget() {}

    virtual void addWidgetToContainer(IFormWidget *) {}
    virtual bool isContainer() const {return false;}

    virtual void createLabel(const QString &text, Qt::Alignment horizAlign);
    virtual QBoxLayout *getBoxLayout(const int labelOption, const QString &text, QWidget *parent);

    virtual void changeEvent(QEvent *event);

    virtual void setLinkedObject(Core::FormItem *link) { m_LinkedObject = link; }
    virtual Core::FormItem *linkedObject() { return m_LinkedObject; }

public Q_SLOTS:
    virtual void retranslate() {}

public:
    QLabel *m_Label;
    Core::FormItem *m_LinkedObject;
    QString m_OldTrans;
};

} // namespace Core

#endif // IFORMWIDGETFACTORY_H
