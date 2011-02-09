#include "accountmanager.h"
#include "ui_accountmanager.h"

accountManager::accountManager(QWidget * parent):QWidget(parent),ui(new Ui::accountManager){
    setupUi(this);
}

accountManager::~accountManager(){}
