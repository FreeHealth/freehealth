#ifndef GIRWIDGET_H
#define GIRWIDGET_H

#include <coreplugin/iformwidgetfactory.h>
#include <QtGui/QWidget>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QAbstractButton;
QT_END_NAMESPACE

namespace Gir {
namespace Internal {

class GirWidgetFactory : public Core::IFormWidgetFactory
{
    Q_OBJECT
public:
    GirWidgetFactory(QObject *parent = 0);
    ~GirWidgetFactory() {}

    bool initialize(const QStringList &, QString *) {return true;}
    bool extensionInitialized() {return true;}
    bool isInitialized() const {return true;}

    bool isContainer(const int idInStringList) const {Q_UNUSED(idInStringList); return false;}
    QStringList providedWidgets() const {return QStringList() << "aggir" << "gir";}
    Core::IFormWidget *createWidget(const QString &name, Core::FormItem *object, QWidget *parent = 0);
};


//--------------------------------------------------------------------------------------------------------
//--------------------------------------- GirWidget implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
namespace Ui {
class GirWidget;
}
class GirUi : public QWidget
{
    Q_OBJECT
public:
    GirUi(QWidget *parent = 0);
    ~GirUi();

public Q_SLOTS:
    void updateGirString(QAbstractButton *radio);
    void on_aButton_clicked();
    void on_bButton_clicked();
    void on_cButton_clicked();

private:
    Ui::GirWidget *m_ui;
    QString m_GirString;
    int m_Gir;
};


class GirWidget : public Core::IFormWidget
{
    Q_OBJECT
public:
    GirWidget(Core::FormItem *linkedObject, QWidget *parent = 0);
    ~GirWidget();

    bool isContainer() const {return false;}

    void setValue(const QVariant &) {}
    QVariant value() const {}

public Q_SLOTS:
    void retranslate() {}

private:
};

} // End Internal
} // End Gir

#endif // GIRWIDGET_H
