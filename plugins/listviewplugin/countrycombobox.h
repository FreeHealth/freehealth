#ifndef VIEWS_COUNTRYCOMBOBOX_H
#define VIEWS_COUNTRYCOMBOBOX_H

#include <listviewplugin/listview_exporter.h>
#include <utils/widgets/countrycombobox.h>

namespace Views {

class LISTVIEW_EXPORT CountryComboBox : public Utils::CountryComboBox
{
    Q_OBJECT
public:
    CountryComboBox(QWidget *parent = 0);

};

}  // End namespace Views

#endif // VIEWS_COUNTRYCOMBOBOX_H
