#ifndef GIRWIDGET_H
#define GIRWIDGET_H
#include <aggirplugin/aggir_exporter.h>
#include <formmanagerplugin/iformwidgetfactory.h>
#include <QtGui/QWidget>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QAbstractButton;
QT_END_NAMESPACE

/**
 * \file girwidget.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 05 Apr 2010
*/


namespace Gir {
namespace Internal {

class GirWidgetFactory : public Form::IFormWidgetFactory
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
    Form::IFormWidget *createWidget(const QString &name, Form::FormItem *object, QWidget *parent = 0);
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


class GirWidget : public Form::IFormWidget
{
    Q_OBJECT
public:
    GirWidget(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~GirWidget();

    bool isContainer() const {return false;}

    void setValue(const QVariant &);
    QVariant value() const;

public Q_SLOTS:
    void retranslate() {}

private:
};

} // End Internal
} // End Gir

#endif // GIRWIDGET_H
