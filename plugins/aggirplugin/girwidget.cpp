#include "girwidget.h"
#include "ui_girwidget.h"

#include <formmanagerplugin/iformitem.h>
#include <medicalutils/aggir/girscore.h>

#include <QDebug>

using namespace Gir;
using namespace Gir::Internal;

GirWidgetFactory::GirWidgetFactory(QObject *parent) :
        IFormWidgetFactory(parent)
{
}

Form::IFormWidget *GirWidgetFactory::createWidget(const QString &name, Form::FormItem *object, QWidget *parent)
{
    if ((name.compare("aggir",Qt::CaseInsensitive)==0) ||
        (name.compare("gir",Qt::CaseInsensitive)==0)) {
        return new GirWidget(object, parent);
    }
    return 0;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------ GirUi -------------------------------------------------
//--------------------------------------------------------------------------------------------------------
GirUi::GirUi(QWidget *parent) :
    QWidget(parent),
    m_ui(0)
{
    m_ui = new Ui::GirWidget();
    m_ui->setupUi(this);
    // connect all QButtonGroup to Gir string preparation
    foreach(QButtonGroup *gr, this->findChildren<QButtonGroup*>()) {
        connect(gr, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(updateGirString(QAbstractButton*)));
    }
    m_GirString.fill('_',13);
    m_Gir = 0;
}

GirUi::~GirUi()
{
    if (m_ui) delete m_ui;
    m_ui=0;
}

inline static void allRadioOn(const QString &s, QWidget *parent)
{
    foreach(QRadioButton *r, parent->findChildren<QRadioButton*>()) {
        if (r->text().compare(s,Qt::CaseInsensitive)==0)
            r->setChecked(true);
    }
}

void GirUi::on_aButton_clicked()
{
    allRadioOn("A",this);
    m_GirString.fill('A',13);
    updateGirString(0);
}

void GirUi::on_bButton_clicked()
{
    allRadioOn("B",this);
    m_GirString.fill('B',13);
    updateGirString(0);
}

void GirUi::on_cButton_clicked()
{
    allRadioOn("C",this);
    m_GirString.fill('C',13);
    updateGirString(0);
}

inline static int calculateGirScore(const QString &s)
{
    MedicalUtils::AGGIR::GirScore score;
    score.setValues(s[0], s[1], s.mid(2,2), s.mid(4,3), s.mid(7,2), s.mid(9,2),s[11], s[12]);
    return score.resultingGir();
}

void GirUi::updateGirString(QAbstractButton *radio)
{
    QButtonGroup *p = 0;
    foreach(QButtonGroup *gr, this->findChildren<QButtonGroup*>()) {
        if (gr->buttons().contains(radio)) {
            p = gr;
            break;
        }
    }
    // all QButtonGroup are name like GR_X where X is the id in the Gir string to fill
    if (p) {
        int id = p->objectName().mid(3).toInt();
        if (id < m_GirString.count())
            m_GirString[id]=radio->text()[0];
    }
    if (!m_GirString.contains("_")) {
        m_Gir = calculateGirScore(m_GirString);
        m_ui->girBar->setValue(7-m_Gir);
        m_ui->girLineEdit->setText(QString::number(m_Gir));
    }
}


//--------------------------------------------------------------------------------------------------------
//---------------------------------------------- GirWidget -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
GirWidget::GirWidget(Form::FormItem *linkedObject, QWidget *parent) :
    Form::IFormWidget(linkedObject,parent)
{
    QBoxLayout * hb = getBoxLayout(Label_OnTop, m_LinkedObject->spec()->label(), this);
    m_Label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    hb->addWidget(m_Label);
    hb->addWidget(new GirUi(this));
    adjustSize();
}

GirWidget::~GirWidget()
{
}

