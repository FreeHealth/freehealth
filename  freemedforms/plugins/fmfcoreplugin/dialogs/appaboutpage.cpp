#include "appaboutpage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QApplication>

using namespace Core;
using namespace Core::Internal;

static const char *ABOUT_TEXT = QT_TRANSLATE_NOOP("AboutDialog",
        "<p align=center><b>Welcome to FreeMedForms</b><br />"
        "(C) 2008-2010 by Eric MAEKER, MD</p>"
        "<p align=left>This application is an early alpha release. This means that a lot of "
        "features are not yet implemented or may not work and that you may experiment bugs.<br />"
        "This software is release without any warranty and only for test purposal.<br />"
        "Please refer to web site for more informations.<br />"
        "<a href=\"%1\">Web site</a>"
        "</p>"
        );

AppAboutPage::AppAboutPage(QObject *parent) :
        IAboutPage(parent)
{
    setObjectName("AppAboutPage");
    m_Widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    label = new QLabel(m_Widget);
    label->setWordWrap(true);
    label->setOpenExternalLinks(true);
    layout->addWidget(label);
    layout->addSpacerItem(new QSpacerItem(20,20, QSizePolicy::Expanding, QSizePolicy::Expanding));
}

AppAboutPage::~AppAboutPage()
{
    if (m_Widget)
        delete m_Widget;
}

void AppAboutPage::refreshContents()
{
    label->clear();
    // add a generic message
    label->setText(tr(ABOUT_TEXT).arg(qApp->organizationDomain()));
}

QWidget *AppAboutPage::widget()
{
    return m_Widget;
}
