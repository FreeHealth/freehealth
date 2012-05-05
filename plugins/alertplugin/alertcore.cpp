#include "alertcore.h"

using namespace Alert;

AlertCore * AlertCore::m_singleton = NULL;

AlertCore::AlertCore(QObject * parent){}

AlertCore::~AlertCore()
{
    if (m_singleton)
    {
    	  m_singleton = NULL;
        }
}

void AlertCore::initialize()
{
    m_alertBase = AlertBase::singleton();
    m_alertManager = AlertManager::singleton();
}

QString AlertCore::setAlertUuid()
{
  return QUuid::createUuid().toString();
}

void AlertCore::showIHMaccordingToType(int type)
{
    m_alertManager->initializeWithType(type);
}
