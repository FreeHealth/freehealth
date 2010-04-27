#include "formplaceholder.h"

#include <utils/widgets/minisplitter.h>

#include <QTreeWidget>
#include <QStackedLayout>

using namespace Form;

namespace Form {
namespace Internal {
class FormPlaceHolderPrivate
{
public:
    FormPlaceHolderPrivate() : m_Tree(0), m_Stack(0), m_GeneralLayout(0)
    {
    }

    ~FormPlaceHolderPrivate()
    {
        delete m_Tree; m_Tree = 0;
        delete m_Stack; m_Stack = 0;
        delete m_GeneralLayout; m_GeneralLayout=0;
    }

    QTreeWidget *m_Tree;
    QStackedLayout *m_Stack;
    QGridLayout *m_GeneralLayout;
};
}
}

FormPlaceHolder::FormPlaceHolder(QWidget *parent) :
        QWidget(parent), d(new Internal::FormPlaceHolderPrivate)
{
    d->m_GeneralLayout = new QGridLayout(this);
    setLayout(d->m_GeneralLayout);

    d->m_Tree = new QTreeWidget(this);
    QWidget *w = new QWidget(this);
    d->m_Stack = new QStackedLayout(this);
    w->setLayout(d->m_Stack);
    Utils::MiniSplitter *s = new Utils::MiniSplitter(this);
    s->setOrientation(Qt::Horizontal);
    s->addWidget(d->m_Tree);
    s->addWidget(w);

    d->m_GeneralLayout->addWidget(s,0,0);

    connect(d->m_Tree, SIGNAL(itemActivated(QTreeWidgetItem*, int)),this,SLOT(changeStackedLayoutTo(QTreeWidgetItem*)));
}

FormPlaceHolder::~FormPlaceHolder()
{
    if (d) {
        delete d;
        d = 0;
    }
}

QTreeWidget *FormPlaceHolder::formTree() const
{
    return d->m_Tree;
}

QStackedLayout *FormPlaceHolder::formStackLayout() const
{
    return d->m_Stack;
}

void FormPlaceHolder::clearFormStackLayout()
{
    for(int i = d->m_Stack->count(); i>0; --i) {
        QWidget *w = d->m_Stack->widget(i);
        d->m_Stack->removeWidget(w);
        delete w;
    }
}

void FormPlaceHolder::addTopWidget(QWidget *top)
{
    d->m_GeneralLayout->addWidget(top, 0, 0, 1, d->m_GeneralLayout->columnCount());
}

void FormPlaceHolder::addBottomWidget(QWidget *bottom)
{
    d->m_GeneralLayout->addWidget(bottom, d->m_GeneralLayout->rowCount(), 0, 1, d->m_GeneralLayout->columnCount());
}

void FormPlaceHolder::changeStackedLayoutTo(QTreeWidgetItem *item)
{
    int id = item->data(0,Qt::UserRole).toInt();
    d->m_Stack->setCurrentIndex(id);
}
