#include "iformwidgetfactory.h"
#include "iformitem.h"

/**
  \class IFormWidgetFactory
 This class is an interface for the form's widget. Object need to be created by plugins and added to
 the plugins manager objects pool.
*/

#include <QLocale>
#include <QEvent>
#include <QBoxLayout>

using namespace Form;


IFormWidget::IFormWidget(Form::FormItem *linkedObject, QWidget *parent )
        : QWidget(parent), m_LinkedObject(linkedObject)
{
}

void IFormWidget::createLabel(const QString &text, Qt::Alignment horizAlign)
{
    m_Label = new QLabel(this);
    QLabel *l = m_Label;
    l->setFrameStyle( IFormWidget::ItemLabelFrame );
    l->setText( text );
    l->setAlignment( Qt::AlignTop | horizAlign );
    l->setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Expanding );
    l->setWordWrap(true);
}

QBoxLayout *IFormWidget::getBoxLayout(const int labelOption, const QString &text, QWidget *parent)
{
    QBoxLayout * hb;
    m_Label = new QLabel(this);
    if (labelOption == Label_OnTop) {
        hb = new QBoxLayout(QBoxLayout::TopToBottom, parent );
        createLabel(text, Qt::AlignTop | Qt::AlignLeft );
        hb->setSpacing( 0 );
    } else {
        /** \todo Code specific label options */
        hb = new QBoxLayout( QBoxLayout::LeftToRight, parent );
        if (labelOption != Label_NoLabel) {
            createLabel(text, Qt::AlignTop | Qt::AlignRight );
        }
    }
    return hb;
}


void IFormWidget::changeEvent(QEvent * event)
{
    QString loc = QLocale().name().left(2);
    if ((event->type() == QEvent::LanguageChange) &&
        (m_OldTrans != loc)) {
        m_OldTrans = loc;
        retranslate();
        event->accept();
    }
    else
        QWidget::changeEvent( event );
}
