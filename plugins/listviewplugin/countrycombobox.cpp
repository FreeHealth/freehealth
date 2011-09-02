#include "countrycombobox.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

using namespace Views;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

CountryComboBox::CountryComboBox(QWidget *parent) :
    Utils::CountryComboBox(parent)
{
    setFlagPath(settings()->path(Core::ISettings::SmallPixmapPath) + "/flags/");
    initialize();
}
