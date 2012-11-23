#include "rapidtofreeviewer.h"
#include "rapidtofreeio.h"

RapidToFreeViewer::RapidToFreeViewer(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    rapidUserComboBox->addItems(rapidcomptamedUsers());
    connect(runButton,SIGNAL(pressed()),this,SLOT(getAndSetDatas()));
    connect(quitButton,SIGNAL(pressed()),this,SLOT(close()));
}

RapidToFreeViewer::~RapidToFreeViewer(){}

void RapidToFreeViewer::getAndSetDatas()
{
    RapidToFreeIO io;
    io.getAndSetAccount();
}

QStringList RapidToFreeViewer::rapidcomptamedUsers()
{
    RapidToFreeIO io;
    return io.getListOfRapidcomptamedUsers();
}

