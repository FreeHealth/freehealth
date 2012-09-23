#include "iamdbcore.h"

using namespace IAMDb;

IAMDbCore *IAMDbCore::_instance = 0;
IAMDbCore *IAMDbCore::instance()
{
    if (!_instance)
        _instance = new IAMDbCore;
    return _instance;
}

IAMDbCore::IAMDbCore()
{
}

bool IAMDbCore::init()
{
    // Create the InteractionStep
    return true;
}

bool IAMDbCore::populateDatabaseWithInteractions(const QString &connection)
{
    Q_UNUSED(connection)
    return true;
}
